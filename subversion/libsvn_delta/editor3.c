/*
 * editor.c :  editing trees of versioned resources
 *
 * ====================================================================
 *    Licensed to the Apache Software Foundation (ASF) under one
 *    or more contributor license agreements.  See the NOTICE file
 *    distributed with this work for additional information
 *    regarding copyright ownership.  The ASF licenses this file
 *    to you under the Apache License, Version 2.0 (the
 *    "License"); you may not use this file except in compliance
 *    with the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing,
 *    software distributed under the License is distributed on an
 *    "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *    KIND, either express or implied.  See the License for the
 *    specific language governing permissions and limitations
 *    under the License.
 * ====================================================================
 */

#include <apr_pools.h>

#include "svn_types.h"
#include "svn_error.h"
#include "svn_pools.h"
#include "svn_dirent_uri.h"

#include "private/svn_editor3.h"

#ifdef SVN_DEBUG
/* This enables runtime checks of the editor API constraints.  This may
   introduce additional memory and runtime overhead, and should not be used
   in production builds. */
#define ENABLE_ORDERING_CHECK
#endif


struct svn_editor3_t
{
  void *baton;

  /* Standard cancellation function. Called before each callback.  */
  svn_cancel_func_t cancel_func;
  void *cancel_baton;

  /* The callback functions.  */
  svn_editor3_cb_funcs_t funcs;

  /* This pool is used as the scratch_pool for all callbacks.  */
  apr_pool_t *scratch_pool;

#ifdef ENABLE_ORDERING_CHECK
  svn_boolean_t within_callback;
  svn_boolean_t finished;
  apr_pool_t *state_pool;
#endif
};


#ifdef ENABLE_ORDERING_CHECK

#define START_CALLBACK(editor)                       \
  do {                                               \
    svn_editor3_t *editor__tmp_e = (editor);          \
    SVN_ERR_ASSERT(!editor__tmp_e->within_callback); \
    editor__tmp_e->within_callback = TRUE;           \
  } while (0)
#define END_CALLBACK(editor) ((editor)->within_callback = FALSE)

#define MARK_FINISHED(editor) ((editor)->finished = TRUE)
#define SHOULD_NOT_BE_FINISHED(editor)  SVN_ERR_ASSERT(!(editor)->finished)

#else

#define START_CALLBACK(editor)  /* empty */
#define END_CALLBACK(editor)  /* empty */

#define MARK_FINISHED(editor)  /* empty */
#define SHOULD_NOT_BE_FINISHED(editor)  /* empty */

#endif /* ENABLE_ORDERING_CHECK */


svn_error_t *
svn_editor3_create(svn_editor3_t **editor,
                   const svn_editor3_cb_funcs_t *editor_funcs,
                   void *editor_baton,
                   svn_cancel_func_t cancel_func,
                   void *cancel_baton,
                   apr_pool_t *result_pool,
                   apr_pool_t *scratch_pool)
{
  *editor = apr_pcalloc(result_pool, sizeof(**editor));

  (*editor)->funcs = *editor_funcs;
  (*editor)->baton = editor_baton;
  (*editor)->cancel_func = cancel_func;
  (*editor)->cancel_baton = cancel_baton;
  (*editor)->scratch_pool = svn_pool_create(result_pool);

#ifdef ENABLE_ORDERING_CHECK
  (*editor)->within_callback = FALSE;
  (*editor)->finished = FALSE;
  (*editor)->state_pool = result_pool;
#endif

  return SVN_NO_ERROR;
}


void *
svn_editor3_get_baton(const svn_editor3_t *editor)
{
  return editor->baton;
}


static svn_error_t *
check_cancel(svn_editor3_t *editor)
{
  svn_error_t *err = NULL;

  if (editor->cancel_func)
    {
      START_CALLBACK(editor);
      err = editor->cancel_func(editor->cancel_baton);
      END_CALLBACK(editor);
    }

  return svn_error_trace(err);
}


/*
 * ===================================================================
 * Editor for Commit (incremental tree changes; path-based addressing)
 * ===================================================================
 */

svn_error_t *
svn_editor3_mk(svn_editor3_t *editor,
               svn_node_kind_t new_kind,
               svn_editor3_txn_path_t parent_loc,
               const char *new_name)
{
  svn_error_t *err = SVN_NO_ERROR;

  /* SVN_ERR_ASSERT(...); */

  SVN_ERR(check_cancel(editor));

  if (editor->funcs.cb_mk)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_mk(editor->baton,
                                new_kind, parent_loc, new_name,
                                editor->scratch_pool);
      END_CALLBACK(editor);
    }

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}

svn_error_t *
svn_editor3_cp(svn_editor3_t *editor,
#ifdef SVN_EDITOR3_WITH_COPY_FROM_THIS_REV
               svn_editor3_txn_path_t from_loc,
#else
               svn_editor3_peg_path_t from_loc,
#endif
               svn_editor3_txn_path_t parent_loc,
               const char *new_name)
{
  svn_error_t *err = SVN_NO_ERROR;

  /* SVN_ERR_ASSERT(...); */

  SVN_ERR(check_cancel(editor));

  if (editor->funcs.cb_cp)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_cp(editor->baton,
                                from_loc, parent_loc, new_name,
                                editor->scratch_pool);
      END_CALLBACK(editor);
    }

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}

svn_error_t *
svn_editor3_mv(svn_editor3_t *editor,
               svn_editor3_peg_path_t from_loc,
               svn_editor3_txn_path_t new_parent_loc,
               const char *new_name)
{
  svn_error_t *err = SVN_NO_ERROR;

  /* SVN_ERR_ASSERT(...); */

  SVN_ERR(check_cancel(editor));

  if (editor->funcs.cb_mv)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_mv(editor->baton,
                                from_loc, new_parent_loc, new_name,
                                editor->scratch_pool);
      END_CALLBACK(editor);
    }

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}

#ifdef SVN_EDITOR3_WITH_RESURRECTION
svn_error_t *
svn_editor3_res(svn_editor3_t *editor,
                svn_editor3_peg_path_t from_loc,
                svn_editor3_txn_path_t parent_loc,
                const char *new_name)
{
  svn_error_t *err = SVN_NO_ERROR;

  /* SVN_ERR_ASSERT(...); */

  SVN_ERR(check_cancel(editor));

  if (editor->funcs.cb_res)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_res(editor->baton,
                                 from_loc, parent_loc, new_name,
                                 editor->scratch_pool);
      END_CALLBACK(editor);
    }

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}
#endif

svn_error_t *
svn_editor3_rm(svn_editor3_t *editor,
               svn_editor3_txn_path_t loc)
{
  svn_error_t *err = SVN_NO_ERROR;

  /* SVN_ERR_ASSERT(...); */

  SVN_ERR(check_cancel(editor));

  if (editor->funcs.cb_rm)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_rm(editor->baton,
                                loc,
                                editor->scratch_pool);
      END_CALLBACK(editor);
    }

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}

svn_error_t *
svn_editor3_put(svn_editor3_t *editor,
                svn_editor3_txn_path_t loc,
                const svn_editor3_node_content_t *new_content)
{
  svn_error_t *err = SVN_NO_ERROR;

  /* SVN_ERR_ASSERT(...); */

  SVN_ERR(check_cancel(editor));

  if (editor->funcs.cb_put)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_put(editor->baton,
                                 loc, new_content,
                                 editor->scratch_pool);
      END_CALLBACK(editor);
    }

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}


/*
 * ========================================================================
 * Editor for Commit (independent per-node changes; node-id addressing)
 * ========================================================================
 */

svn_error_t *
svn_editor3_add(svn_editor3_t *editor,
                svn_editor3_nbid_t local_nbid,
                svn_node_kind_t new_kind,
                svn_editor3_nbid_t new_parent_nbid,
                const char *new_name,
                const svn_editor3_node_content_t *new_content)
{
  svn_error_t *err = SVN_NO_ERROR;

  /* SVN_ERR_ASSERT(...); */

  SVN_ERR(check_cancel(editor));

  if (editor->funcs.cb_add)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_add(editor->baton,
                                 local_nbid, new_kind,
                                 new_parent_nbid, new_name,
                                 new_content,
                                 editor->scratch_pool);
      END_CALLBACK(editor);
    }

  /* MARK_...(editor, ...); */

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}

svn_error_t *
svn_editor3_copy_one(svn_editor3_t *editor,
                     svn_editor3_nbid_t local_nbid,
                     svn_revnum_t src_revision,
                     svn_editor3_nbid_t src_nbid,
                     svn_editor3_nbid_t new_parent_nbid,
                     const char *new_name,
                     const svn_editor3_node_content_t *new_content)
{
  svn_error_t *err = SVN_NO_ERROR;

  /* SVN_ERR_ASSERT(...); */

  SVN_ERR(check_cancel(editor));

  if (editor->funcs.cb_copy_one)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_copy_one(editor->baton,
                                      local_nbid,
                                      src_revision, src_nbid,
                                      new_parent_nbid, new_name,
                                      new_content,
                                      editor->scratch_pool);
      END_CALLBACK(editor);
    }

  /* MARK_...(editor, ...); */

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}

svn_error_t *
svn_editor3_copy_tree(svn_editor3_t *editor,
                      svn_revnum_t src_revision,
                      svn_editor3_nbid_t src_nbid,
                      svn_editor3_nbid_t new_parent_nbid,
                      const char *new_name)
{
  svn_error_t *err = SVN_NO_ERROR;

  /* SVN_ERR_ASSERT(...); */

  SVN_ERR(check_cancel(editor));

  if (editor->funcs.cb_copy_one)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_copy_tree(editor->baton,
                                       src_revision, src_nbid,
                                       new_parent_nbid, new_name,
                                       editor->scratch_pool);
      END_CALLBACK(editor);
    }

  /* MARK_...(editor, ...); */

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}

svn_error_t *
svn_editor3_delete(svn_editor3_t *editor,
                   svn_revnum_t since_rev,
                   svn_editor3_nbid_t nbid)
{
  svn_error_t *err = SVN_NO_ERROR;

  /* SVN_ERR_ASSERT(...); */

  SVN_ERR(check_cancel(editor));

  if (editor->funcs.cb_delete)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_delete(editor->baton,
                                    since_rev, nbid,
                                    editor->scratch_pool);
      END_CALLBACK(editor);
    }

  /* MARK_...(editor, ...); */

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}

svn_error_t *
svn_editor3_alter(svn_editor3_t *editor,
                  svn_revnum_t since_rev,
                  svn_editor3_nbid_t nbid,
                  svn_editor3_nbid_t new_parent_nbid,
                  const char *new_name,
                  const svn_editor3_node_content_t *new_content)
{
  svn_error_t *err = SVN_NO_ERROR;

  /* SVN_ERR_ASSERT(...); */

  SVN_ERR(check_cancel(editor));

  if (editor->funcs.cb_alter)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_alter(editor->baton,
                                   since_rev, nbid,
                                   new_parent_nbid, new_name,
                                   new_content,
                                   editor->scratch_pool);
      END_CALLBACK(editor);
    }

  /* MARK_...(editor, ...); */

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}

svn_error_t *
svn_editor3_complete(svn_editor3_t *editor)
{
  svn_error_t *err = SVN_NO_ERROR;

  SHOULD_NOT_BE_FINISHED(editor);

  if (editor->funcs.cb_complete)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_complete(editor->baton, editor->scratch_pool);
      END_CALLBACK(editor);
    }

  MARK_FINISHED(editor);

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}

svn_error_t *
svn_editor3_abort(svn_editor3_t *editor)
{
  svn_error_t *err = SVN_NO_ERROR;

  SHOULD_NOT_BE_FINISHED(editor);

  if (editor->funcs.cb_abort)
    {
      START_CALLBACK(editor);
      err = editor->funcs.cb_abort(editor->baton, editor->scratch_pool);
      END_CALLBACK(editor);
    }

  MARK_FINISHED(editor);

  svn_pool_clear(editor->scratch_pool);
  return svn_error_trace(err);
}

svn_editor3_node_content_t *
svn_editor3_node_content_create_ref(svn_editor3_peg_path_t ref,
                                    apr_pool_t *result_pool)
{
  svn_editor3_node_content_t *new_content
    = apr_pcalloc(result_pool, sizeof(*new_content));

  new_content->kind = svn_node_unknown;
  new_content->ref = ref;
  return new_content;
}

svn_editor3_node_content_t *
svn_editor3_node_content_create_dir(svn_editor3_peg_path_t ref,
                                    apr_hash_t *props,
                                    apr_pool_t *result_pool)
{
  svn_editor3_node_content_t *new_content
    = apr_pcalloc(result_pool, sizeof(*new_content));

  new_content->kind = svn_node_dir;
  new_content->ref = ref;
  new_content->props = props;
  return new_content;
}

svn_editor3_node_content_t *
svn_editor3_node_content_create_file(svn_editor3_peg_path_t ref,
                                     apr_hash_t *props,
                                     const svn_checksum_t *checksum,
                                     svn_stream_t *stream,
                                     apr_pool_t *result_pool)
{
  svn_editor3_node_content_t *new_content
    = apr_pcalloc(result_pool, sizeof(*new_content));

  new_content->kind = svn_node_file;
  new_content->ref = ref;
  new_content->props = props;
  new_content->checksum = checksum;
  new_content->stream = stream;
  return new_content;
}

svn_editor3_node_content_t *
svn_editor3_node_content_create_symlink(svn_editor3_peg_path_t ref,
                                        apr_hash_t *props,
                                        const char *target,
                                        apr_pool_t *result_pool)
{
  svn_editor3_node_content_t *new_content
    = apr_pcalloc(result_pool, sizeof(*new_content));

  new_content->kind = svn_node_symlink;
  new_content->ref = ref;
  new_content->props = props;
  new_content->target = target;
  return new_content;
}


#ifdef SVN_DEBUG

/*
 * ===================================================================
 * A wrapper editor that forwards calls through to a wrapped editor
 * while printing a diagnostic trace of the calls.
 * ===================================================================
 */

typedef struct wrapper_baton_t
{
  svn_editor3_t *wrapped_editor;

  /* debug printing stream */
  svn_stream_t *debug_stream;
  /* debug printing prefix*/
  const char *prefix;

} wrapper_baton_t;

/* Print the variable arguments, formatted with FMT like with 'printf',
 * to the stream EB->debug_stream, prefixed with EB->prefix. */
static void
dbg(wrapper_baton_t *eb,
    apr_pool_t *scratch_pool,
    const char *fmt,
    ...)
{
  const char *message;
  va_list ap;

  va_start(ap, fmt);
  message = apr_pvsprintf(scratch_pool, fmt, ap);
  va_end(ap);

  if (eb->prefix)
    svn_error_clear(svn_stream_puts(eb->debug_stream, eb->prefix));
  svn_error_clear(svn_stream_puts(eb->debug_stream, message));
  svn_error_clear(svn_stream_puts(eb->debug_stream, "\n"));
}

/* Return a human-readable string representation of LOC. */
static const char *
peg_path_str(svn_editor3_peg_path_t loc,
             apr_pool_t *result_pool)
{
  return apr_psprintf(result_pool, "%s@%ld",
                      loc.relpath, loc.rev);
}

/* Return a human-readable string representation of LOC. */
static const char *
txn_path_str(svn_editor3_txn_path_t loc,
             apr_pool_t *result_pool)
{
  return apr_psprintf(result_pool, "%s//%s",
                      peg_path_str(loc.peg, result_pool), loc.relpath);
}

/* Return a human-readable string representation of NBID. */
static const char *
nbid_str(svn_editor3_nbid_t nbid,
         apr_pool_t *result_pool)
{
  return txn_path_str(nbid, result_pool);
}

static svn_error_t *
wrap_mk(void *baton,
        svn_node_kind_t new_kind,
        svn_editor3_txn_path_t parent_loc,
        const char *new_name,
        apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "mk(k=%s, p=%s, n=%s)",
      svn_node_kind_to_word(new_kind),
      txn_path_str(parent_loc, scratch_pool), new_name);
  SVN_ERR(svn_editor3_mk(eb->wrapped_editor,
                         new_kind, parent_loc, new_name));
  return SVN_NO_ERROR;
}

static svn_error_t *
wrap_cp(void *baton,
#ifdef SVN_EDITOR3_WITH_COPY_FROM_THIS_REV
        svn_editor3_txn_path_t from_loc,
#else
        svn_editor3_peg_path_t from_loc,
#endif
        svn_editor3_txn_path_t parent_loc,
        const char *new_name,
        apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "cp(f=%s, p=%s, n=%s)",
      peg_path_str(from_loc, scratch_pool),
      txn_path_str(parent_loc, scratch_pool), new_name);
  SVN_ERR(svn_editor3_cp(eb->wrapped_editor,
                         from_loc, parent_loc, new_name));
  return SVN_NO_ERROR;
}

static svn_error_t *
wrap_mv(void *baton,
        svn_editor3_peg_path_t from_loc,
        svn_editor3_txn_path_t new_parent_loc,
        const char *new_name,
        apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "mv(f=%s, p=%s, n=%s)",
      peg_path_str(from_loc, scratch_pool),
      txn_path_str(new_parent_loc, scratch_pool), new_name);
  SVN_ERR(svn_editor3_mv(eb->wrapped_editor,
                         from_loc, new_parent_loc, new_name));
  return SVN_NO_ERROR;
}

#ifdef SVN_EDITOR3_WITH_RESURRECTION
static svn_error_t *
wrap_res(void *baton,
         svn_editor3_peg_path_t from_loc,
         svn_editor3_txn_path_t parent_loc,
         const char *new_name,
         apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "res(f=%s, p=%s, n=%s)",
      peg_path_str(from_loc, scratch_pool),
      txn_path_str(parent_loc, scratch_pool), new_name);
  SVN_ERR(svn_editor3_res(eb->wrapped_editor,
                          from_loc, parent_loc, new_name));
  return SVN_NO_ERROR;
}
#endif

static svn_error_t *
wrap_rm(void *baton,
        svn_editor3_txn_path_t loc,
        apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "rm(%s)",
      txn_path_str(loc, scratch_pool));
  SVN_ERR(svn_editor3_rm(eb->wrapped_editor,
                         loc));
  return SVN_NO_ERROR;
}

static svn_error_t *
wrap_put(void *baton,
         svn_editor3_txn_path_t loc,
         const svn_editor3_node_content_t *new_content,
         apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "put(%s)",
      txn_path_str(loc, scratch_pool));
  SVN_ERR(svn_editor3_put(eb->wrapped_editor,
                          loc, new_content));
  return SVN_NO_ERROR;
}

static svn_error_t *
wrap_add(void *baton,
         svn_editor3_nbid_t local_nbid,
         svn_node_kind_t new_kind,
         svn_editor3_nbid_t new_parent_nbid,
         const char *new_name,
         const svn_editor3_node_content_t *new_content,
         apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "%s : add(k=%s, p=%s, n=%s, c=...)",
      nbid_str(local_nbid, scratch_pool), svn_node_kind_to_word(new_kind),
      nbid_str(new_parent_nbid, scratch_pool), new_name);
  SVN_ERR(svn_editor3_add(eb->wrapped_editor,
                          local_nbid, new_kind,
                          new_parent_nbid, new_name, new_content));
  return SVN_NO_ERROR;
}

static svn_error_t *
wrap_copy_one(void *baton,
              svn_editor3_nbid_t local_nbid,
              svn_revnum_t src_revision,
              svn_editor3_nbid_t src_nbid,
              svn_editor3_nbid_t new_parent_nbid,
              const char *new_name,
              const svn_editor3_node_content_t *new_content,
              apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "%s : copy_one(f=%s, p=%s, n=%s, c=...)",
      nbid_str(local_nbid, scratch_pool), nbid_str(src_nbid, scratch_pool),
      nbid_str(new_parent_nbid, scratch_pool), new_name);
  SVN_ERR(svn_editor3_copy_one(eb->wrapped_editor,
                               local_nbid, src_revision, src_nbid,
                               new_parent_nbid, new_name, new_content));
  return SVN_NO_ERROR;
}

static svn_error_t *
wrap_copy_tree(void *baton,
               svn_revnum_t src_revision,
               svn_editor3_nbid_t src_nbid,
               svn_editor3_nbid_t new_parent_nbid,
               const char *new_name,
               apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "... : copy_tree(f=%s, p=%s, n=%s)",
      nbid_str(src_nbid, scratch_pool),
      nbid_str(new_parent_nbid, scratch_pool), new_name);
  SVN_ERR(svn_editor3_copy_tree(eb->wrapped_editor,
                                src_revision, src_nbid,
                                new_parent_nbid, new_name));
  return SVN_NO_ERROR;
}

static svn_error_t *
wrap_delete(void *baton,
            svn_revnum_t since_rev,
            svn_editor3_nbid_t nbid,
            apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "%s : delete()",
      nbid_str(nbid, scratch_pool));
  SVN_ERR(svn_editor3_delete(eb->wrapped_editor,
                             since_rev, nbid));
  return SVN_NO_ERROR;
}

static svn_error_t *
wrap_alter(void *baton,
           svn_revnum_t since_rev,
           svn_editor3_nbid_t nbid,
           svn_editor3_nbid_t new_parent_nbid,
           const char *new_name,
           const svn_editor3_node_content_t *new_content,
           apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "%s : alter(p=%s, n=%s, c=...)",
      nbid_str(nbid, scratch_pool), nbid_str(nbid, scratch_pool),
      nbid_str(new_parent_nbid, scratch_pool), new_name);
  SVN_ERR(svn_editor3_alter(eb->wrapped_editor,
                            since_rev, nbid,
                            new_parent_nbid, new_name, new_content));
  return SVN_NO_ERROR;
}

static svn_error_t *
wrap_complete(void *baton,
              apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "complete()");
  SVN_ERR(svn_editor3_complete(eb->wrapped_editor));
  return SVN_NO_ERROR;
}

static svn_error_t *
wrap_abort(void *baton,
           apr_pool_t *scratch_pool)
{
  wrapper_baton_t *eb = baton;

  dbg(eb, scratch_pool, "abort()");
  SVN_ERR(svn_editor3_abort(eb->wrapped_editor));
  return SVN_NO_ERROR;
}

svn_error_t *
svn_editor3__get_debug_editor(svn_editor3_t **editor_p,
                              svn_editor3_t *wrapped_editor,
                              apr_pool_t *result_pool)
{
  apr_pool_t *scratch_pool = result_pool;
  static const svn_editor3_cb_funcs_t wrapper_funcs = {
    wrap_mk,
    wrap_cp,
    wrap_mv,
#ifdef SVN_EDITOR3_WITH_RESURRECTION
    wrap_res,
#endif
    wrap_rm,
    wrap_put,
    wrap_add,
    wrap_copy_one,
    wrap_copy_tree,
    wrap_delete,
    wrap_alter,
    wrap_complete,
    wrap_abort
  };
  wrapper_baton_t *eb = apr_palloc(result_pool, sizeof(*eb));

  eb->wrapped_editor = wrapped_editor;

  /* set up for diagnostic printing */
  {
    apr_file_t *errfp;
    apr_status_t apr_err = apr_file_open_stdout(&errfp, result_pool);

    if (apr_err)
      return svn_error_wrap_apr(apr_err, "Failed to open debug output stream");

    eb->debug_stream = svn_stream_from_aprfile2(errfp, TRUE, result_pool);
    eb->prefix = apr_pstrdup(result_pool, "DBG: ");
  }

  SVN_ERR(svn_editor3_create(editor_p, &wrapper_funcs, eb,
                             NULL, NULL, /* cancellation */
                             result_pool, scratch_pool));

  return SVN_NO_ERROR;
}
#endif
