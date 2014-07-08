/* Calculate the repository relative path of DIFF_RELPATH, using RA_SESSION
 * and WC_CTX, and return the result in *REPOS_RELPATH.
                   svn_ra_session_t *ra_session,
  const char *orig_repos_relpath = NULL;
  if (! ra_session
      if (!ra_session
  {
    const char *url;
    const char *repos_root_url;

    /* Would be nice if the RA layer could just provide the parent
       repos_relpath of the ra session */
      SVN_ERR(svn_ra_get_session_url(ra_session, &url, scratch_pool));

      SVN_ERR(svn_ra_get_repos_root2(ra_session, &repos_root_url,
                                     scratch_pool));

      orig_repos_relpath = svn_uri_skip_ancestor(repos_root_url, url,
                                                 scratch_pool);

      *repos_relpath = svn_relpath_join(orig_repos_relpath, diff_relpath,
                                        result_pool);
  }
       simply join diff_cmd_baton->orig_path_1 with path, ditto for
    new_path1 = apr_psprintf(result_pool, "%s\t(%s)", new_path, new_path2);
  if (revnum != SVN_INVALID_REVNUM)
  else
                   svn_ra_session_t *ra_session,
                                 ra_session, wc_ctx, anchor,
                                 ra_session, wc_ctx, anchor,
struct diff_cmd_baton {
  /* The original targets passed to the diff command.  We may need
     these to construct distinctive diff labels when comparing the
     same relative path in the same revision, under different anchors
     (for example, when comparing a trunk against a branch). */
  const char *orig_path_1;
  const char *orig_path_2;

  /* These are the numeric representations of the revisions passed to
     svn_client_diff6(), either may be SVN_INVALID_REVNUM.  We need these
     because some of the svn_wc_diff_callbacks4_t don't get revision
     arguments.

     ### Perhaps we should change the callback signatures and eliminate
     ### these?
  */
  svn_revnum_t revnum1;
  svn_revnum_t revnum2;

  svn_boolean_t no_copyfrom_on_add;
  /* The RA session used during diffs involving the repository. */
  svn_ra_session_t *ra_session;

  /* The anchor to prefix before wc paths */
  const char *anchor;
  /* Whether the local diff target of a repos->wc diff is a copy. */
  svn_boolean_t repos_wc_diff_target_is_copy;
};
                   svn_boolean_t dir_was_added,
                   struct diff_cmd_baton *diff_cmd_baton,
  if (diff_cmd_baton->ignore_properties)
      /* We're using the revnums from the diff_cmd_baton since there's
                                 diff_cmd_baton->anchor,
                                 diff_cmd_baton->orig_path_1,
                                 diff_cmd_baton->orig_path_2,
                                 diff_cmd_baton->header_encoding,
                                 diff_cmd_baton->outstream,
                                 diff_cmd_baton->relative_to_dir,
                                 diff_cmd_baton->use_git_diff_format,
                                 diff_cmd_baton->ra_session,
                                 diff_cmd_baton->wc_ctx,
/* An svn_wc_diff_callbacks4_t function. */
static svn_error_t *
diff_dir_props_changed(svn_wc_notify_state_t *state,
                       svn_boolean_t *tree_conflicted,
                       const char *diff_relpath,
                       svn_boolean_t dir_was_added,
                       const apr_array_header_t *propchanges,
                       apr_hash_t *original_props,
                       void *diff_baton,
                       apr_pool_t *scratch_pool)
{
  struct diff_cmd_baton *diff_cmd_baton = diff_baton;

  return svn_error_trace(diff_props_changed(diff_relpath,
                                            /* ### These revs be filled
                                             * ### with per node info */
                                            dir_was_added
                                                ? 0 /* Magic legacy value */
                                                : diff_cmd_baton->revnum1,
                                            diff_cmd_baton->revnum2,
                                            dir_was_added,
                                            propchanges,
                                            original_props,
                                            TRUE /* show_diff_header */,
                                            diff_cmd_baton,
                                            scratch_pool));
}


                     struct diff_cmd_baton *diff_cmd_baton,
  int exitcode;
  const char *rel_to_dir = diff_cmd_baton->relative_to_dir;
  svn_stream_t *errstream = diff_cmd_baton->errstream;
  svn_stream_t *outstream = diff_cmd_baton->outstream;
  const char *path1 = diff_cmd_baton->orig_path_1;
  const char *path2 = diff_cmd_baton->orig_path_2;
  if (diff_cmd_baton->properties_only)
                                       rel_to_dir, diff_cmd_baton->anchor,
  if (! diff_cmd_baton->force_binary && (mt1_binary || mt2_binary))
               diff_cmd_baton->header_encoding, scratch_pool,
      SVN_ERR(svn_stream_printf_from_utf8(outstream,
               diff_cmd_baton->header_encoding, scratch_pool,
               _("Cannot display: file marked as a binary type.%s"),
               APR_EOL_STR));

      if (mt1_binary && !mt2_binary)
        SVN_ERR(svn_stream_printf_from_utf8(outstream,
                 diff_cmd_baton->header_encoding, scratch_pool,
                 "svn:mime-type = %s" APR_EOL_STR, mimetype1));
      else if (mt2_binary && !mt1_binary)
        SVN_ERR(svn_stream_printf_from_utf8(outstream,
                 diff_cmd_baton->header_encoding, scratch_pool,
                 "svn:mime-type = %s" APR_EOL_STR, mimetype2));
      else if (mt1_binary && mt2_binary)
          if (strcmp(mimetype1, mimetype2) == 0)
                     diff_cmd_baton->header_encoding, scratch_pool,
                     "svn:mime-type = %s" APR_EOL_STR,
                     mimetype1));
          else
                     diff_cmd_baton->header_encoding, scratch_pool,
                     "svn:mime-type = (%s, %s)" APR_EOL_STR,
                     mimetype1, mimetype2));
  if (diff_cmd_baton->diff_cmd)
               diff_cmd_baton->header_encoding, scratch_pool,
                               diff_cmd_baton->options.for_external.argv,
                               diff_cmd_baton->options.for_external.argc,
                               diff_cmd_baton->diff_cmd, scratch_pool));
                                   diff_cmd_baton->options.for_internal,
          || diff_cmd_baton->use_git_diff_format
                   diff_cmd_baton->header_encoding, scratch_pool,
          if (diff_cmd_baton->use_git_diff_format)
                                         diff_cmd_baton->orig_path_1,
                                         diff_cmd_baton->ra_session,
                                         diff_cmd_baton->wc_ctx,
                                         diff_cmd_baton->anchor,
                                         diff_cmd_baton->orig_path_2,
                                         diff_cmd_baton->ra_session,
                                         diff_cmd_baton->wc_ctx,
                                         diff_cmd_baton->anchor,
                                            diff_cmd_baton->header_encoding,
            SVN_ERR(svn_diff_file_output_unified3(outstream, diff,
                     diff_cmd_baton->header_encoding, rel_to_dir,
                     diff_cmd_baton->options.for_internal->show_c_function,
diff_file_opened(svn_boolean_t *tree_conflicted,
                 svn_boolean_t *skip,
                 const char *diff_relpath,
                 svn_revnum_t rev,
                 void *diff_baton,
                 apr_pool_t *scratch_pool)
{
  return SVN_NO_ERROR;
}

/* An svn_wc_diff_callbacks4_t function. */
static svn_error_t *
diff_file_changed(svn_wc_notify_state_t *content_state,
                  svn_wc_notify_state_t *prop_state,
                  svn_boolean_t *tree_conflicted,
                  const char *diff_relpath,
                  const char *tmpfile1,
                  const char *tmpfile2,
                  svn_revnum_t rev1,
                  svn_revnum_t rev2,
                  const char *mimetype1,
                  const char *mimetype2,
                  apr_hash_t *original_props,
                  void *diff_baton,
  struct diff_cmd_baton *diff_cmd_baton = diff_baton;
  /* During repos->wc diff of a copy revision numbers obtained
   * from the working copy are always SVN_INVALID_REVNUM. */
  if (diff_cmd_baton->repos_wc_diff_target_is_copy)
    {
      if (rev1 == SVN_INVALID_REVNUM &&
          diff_cmd_baton->revnum1 != SVN_INVALID_REVNUM)
        rev1 = diff_cmd_baton->revnum1;

      if (rev2 == SVN_INVALID_REVNUM &&
          diff_cmd_baton->revnum2 != SVN_INVALID_REVNUM)
        rev2 = diff_cmd_baton->revnum2;
    }

  if (tmpfile1)
    SVN_ERR(diff_content_changed(&wrote_header, diff_relpath,
                                 tmpfile1, tmpfile2, rev1, rev2,
                                 mimetype1, mimetype2,
                                 SVN_INVALID_REVNUM, diff_cmd_baton,
    SVN_ERR(diff_props_changed(diff_relpath, rev1, rev2, FALSE, prop_changes,
                               original_props, !wrote_header,
                               diff_cmd_baton, scratch_pool));
/* An svn_wc_diff_callbacks4_t function. */
diff_file_added(svn_wc_notify_state_t *content_state,
                svn_wc_notify_state_t *prop_state,
                svn_boolean_t *tree_conflicted,
                const char *diff_relpath,
                const char *tmpfile1,
                const char *tmpfile2,
                svn_revnum_t rev1,
                svn_revnum_t rev2,
                const char *mimetype1,
                const char *mimetype2,
                const char *copyfrom_path,
                svn_revnum_t copyfrom_revision,
                const apr_array_header_t *prop_changes,
                apr_hash_t *original_props,
                void *diff_baton,
  struct diff_cmd_baton *diff_cmd_baton = diff_baton;
  if (diff_cmd_baton->repos_wc_diff_target_is_copy)
      if (rev1 == SVN_INVALID_REVNUM &&
          diff_cmd_baton->revnum1 != SVN_INVALID_REVNUM)
        rev1 = diff_cmd_baton->revnum1;

      if (rev2 == SVN_INVALID_REVNUM &&
          diff_cmd_baton->revnum2 != SVN_INVALID_REVNUM)
        rev2 = diff_cmd_baton->revnum2;

  if (diff_cmd_baton->no_copyfrom_on_add
      && (copyfrom_path || SVN_IS_VALID_REVNUM(copyfrom_revision)))
      apr_hash_t *empty_hash = apr_hash_make(scratch_pool);
      apr_array_header_t *new_changes;

      /* Rebase changes on having no left source. */
      if (!diff_cmd_baton->empty_file)
        SVN_ERR(svn_io_open_unique_file3(NULL, &diff_cmd_baton->empty_file,
                                         diff_cmd_baton->pool, scratch_pool));
      SVN_ERR(svn_prop_diffs(&new_changes,
                             svn_prop__patch(original_props, prop_changes,
                                             scratch_pool),
                             empty_hash,
                             scratch_pool));
      tmpfile1 = diff_cmd_baton->empty_file;
      prop_changes = new_changes;
      original_props = empty_hash;
      copyfrom_revision = SVN_INVALID_REVNUM;
  if (diff_cmd_baton->no_diff_added)
      const char *index_path = diff_relpath;
      if (diff_cmd_baton->anchor)
        index_path = svn_dirent_join(diff_cmd_baton->anchor, diff_relpath,
      SVN_ERR(svn_stream_printf_from_utf8(diff_cmd_baton->outstream,
                diff_cmd_baton->header_encoding, scratch_pool,
  else if (tmpfile1 && copyfrom_path)
    SVN_ERR(diff_content_changed(&wrote_header, diff_relpath,
                                 tmpfile1, tmpfile2, rev1, rev2,
                                 mimetype1, mimetype2,
                                 copyfrom_path,
                                 copyfrom_revision, diff_cmd_baton,
                                 scratch_pool));
  else if (tmpfile1)
    SVN_ERR(diff_content_changed(&wrote_header, diff_relpath,
                                 tmpfile1, tmpfile2, rev1, rev2,
                                 mimetype1, mimetype2,
                                 diff_cmd_baton, scratch_pool));
    SVN_ERR(diff_props_changed(diff_relpath, rev1, rev2,
                               FALSE, prop_changes,
                               original_props, ! wrote_header,
                               diff_cmd_baton, scratch_pool));
/* An svn_wc_diff_callbacks4_t function. */
diff_file_deleted(svn_wc_notify_state_t *state,
                  svn_boolean_t *tree_conflicted,
                  const char *diff_relpath,
                  const char *tmpfile1,
                  const char *tmpfile2,
                  const char *mimetype1,
                  const char *mimetype2,
                  apr_hash_t *original_props,
                  void *diff_baton,
  struct diff_cmd_baton *diff_cmd_baton = diff_baton;
  if (diff_cmd_baton->no_diff_deleted)
      const char *index_path = diff_relpath;
      if (diff_cmd_baton->anchor)
        index_path = svn_dirent_join(diff_cmd_baton->anchor, diff_relpath,
      SVN_ERR(svn_stream_printf_from_utf8(diff_cmd_baton->outstream,
                diff_cmd_baton->header_encoding, scratch_pool,
      if (tmpfile1)
        SVN_ERR(diff_content_changed(&wrote_header, diff_relpath,
                                     tmpfile1, tmpfile2,
                                     diff_cmd_baton->revnum1,
                                     diff_cmd_baton->revnum2,
                                     mimetype1, mimetype2,
                                     diff_cmd_baton,
      /* Should we also report the properties as deleted? */
  /* We don't list all the deleted properties. */

diff_dir_added(svn_wc_notify_state_t *state,
               svn_boolean_t *tree_conflicted,
               svn_boolean_t *skip,
               svn_boolean_t *skip_children,
               const char *diff_relpath,
               svn_revnum_t rev,
               const char *copyfrom_path,
               svn_revnum_t copyfrom_revision,
               void *diff_baton,
               apr_pool_t *scratch_pool)
  /* Do nothing. */
/* An svn_wc_diff_callbacks4_t function. */
diff_dir_deleted(svn_wc_notify_state_t *state,
                 svn_boolean_t *tree_conflicted,
                 const char *diff_relpath,
                 void *diff_baton,
                 apr_pool_t *scratch_pool)
  /* Do nothing. */
  return SVN_NO_ERROR;
/* An svn_wc_diff_callbacks4_t function. */
diff_dir_opened(svn_boolean_t *tree_conflicted,
                svn_boolean_t *skip,
                svn_boolean_t *skip_children,
                const char *diff_relpath,
                svn_revnum_t rev,
                void *diff_baton,
                apr_pool_t *scratch_pool)
  /* Do nothing. */
  return SVN_NO_ERROR;
}
/* An svn_wc_diff_callbacks4_t function. */
static svn_error_t *
diff_dir_closed(svn_wc_notify_state_t *contentstate,
                svn_wc_notify_state_t *propstate,
                svn_boolean_t *tree_conflicted,
                const char *diff_relpath,
                svn_boolean_t dir_was_added,
                void *diff_baton,
                apr_pool_t *scratch_pool)
{
  /* Do nothing. */
static const svn_wc_diff_callbacks4_t diff_callbacks =
{
  diff_file_opened,
  diff_file_changed,
  diff_file_added,
  diff_file_deleted,
  diff_dir_deleted,
  diff_dir_opened,
  diff_dir_added,
  diff_dir_props_changed,
  diff_dir_closed
};


/* Return in *RESOLVED_URL the URL which PATH_OR_URL@PEG_REVISION has in
 * REVISION. If the object has no location in REVISION, set *RESOLVED_URL
 * to NULL. */
static svn_error_t *
resolve_pegged_diff_target_url(const char **resolved_url,
                               svn_ra_session_t *ra_session,
                               const char *path_or_url,
                               const svn_opt_revision_t *peg_revision,
                               const svn_opt_revision_t *revision,
                               svn_client_ctx_t *ctx,
                               apr_pool_t *scratch_pool)
{
  svn_error_t *err;

  /* Check if the PATH_OR_URL exists at REVISION. */
  err = svn_client__repos_locations(resolved_url, NULL,
                                    NULL, NULL,
                                    ra_session,
                                    path_or_url,
                                    peg_revision,
                                    revision,
                                    NULL,
                                    ctx, scratch_pool);
  if (err)
    {
      if (err->apr_err == SVN_ERR_CLIENT_UNRELATED_RESOURCES ||
          err->apr_err == SVN_ERR_FS_NOT_FOUND)
        {
          svn_error_clear(err);
          *resolved_url = NULL;
        }
      else
        return svn_error_trace(err);
    }

  return SVN_NO_ERROR;
}

 * Set *BASE_PATH corresponding to the URL opened in the new *RA_SESSION
 * which is pointing at *ANCHOR1.
                         const char **base_path,
  const char *abspath_or_url2;
  const char *abspath_or_url1;
      SVN_ERR(svn_dirent_get_absolute(&abspath_or_url2, path_or_url2, pool));
      SVN_ERR(svn_wc__node_get_url(url2, ctx->wc_ctx, abspath_or_url2,
      wri_abspath = abspath_or_url2;
    *url2 = abspath_or_url2 = apr_pstrdup(pool, path_or_url2);
      SVN_ERR(svn_dirent_get_absolute(&abspath_or_url1, path_or_url1, pool));
      SVN_ERR(svn_wc__node_get_url(url1, ctx->wc_ctx, abspath_or_url1,
                                   pool, pool));
      wri_abspath = abspath_or_url1;
  else
    *url1 = abspath_or_url1 = apr_pstrdup(pool, path_or_url1);

  /* We need exactly one BASE_PATH, so we'll let the BASE_PATH
     calculated for PATH_OR_URL2 override the one for PATH_OR_URL1
     (since the diff will be "applied" to URL2 anyway). */
  *base_path = NULL;
  if (strcmp(*url1, path_or_url1) != 0)
    *base_path = path_or_url1;
  if (strcmp(*url2, path_or_url2) != 0)
    *base_path = path_or_url2;
  if (peg_revision->kind != svn_opt_revision_unspecified)
      const char *resolved_url1;
      const char *resolved_url2;

      SVN_ERR(resolve_pegged_diff_target_url(&resolved_url2, *ra_session,
                                             path_or_url2, peg_revision,
                                             revision2, ctx, pool));

      SVN_ERR(svn_ra_reparent(*ra_session, *url1, pool));
      SVN_ERR(resolve_pegged_diff_target_url(&resolved_url1, *ra_session,
                                             path_or_url1, peg_revision,
                                             revision1, ctx, pool));

      /* Either or both URLs might have changed as a result of resolving
       * the PATH_OR_URL@PEG_REVISION's history. If only one of the URLs
       * could be resolved, use the same URL for URL1 and URL2, so we can
       * show a diff that adds or removes the object (see issue #4153). */
      if (resolved_url2)
          *url2 = resolved_url2;
          if (!resolved_url1)
            *url1 = resolved_url2;
      if (resolved_url1)
          *url1 = resolved_url1;
          if (!resolved_url2)
            *url2 = resolved_url1;
      /* Reparent the session, since *URL2 might have changed as a result
         the above call. */
      SVN_ERR(svn_ra_reparent(*ra_session, *url2, pool));
  SVN_ERR(svn_client__get_revision_number(rev2, NULL, ctx->wc_ctx,
           (path_or_url2 == *url2) ? NULL : abspath_or_url2,
           *ra_session, revision2, pool));
  SVN_ERR(svn_client__get_revision_number(rev1, NULL, ctx->wc_ctx,
           (strcmp(path_or_url1, *url1) == 0) ? NULL : abspath_or_url1,
           *ra_session, revision1, pool));
      if (*base_path
          && (*kind1 == svn_node_file || *kind2 == svn_node_file))
        *base_path = svn_dirent_dirname(*base_path, pool);
   comparisions between nodes in the working copy.
diff_wc_wc(const char *path1,
           svn_boolean_t show_copies_as_adds,
           svn_boolean_t use_git_diff_format,
           const svn_wc_diff_callbacks4_t *callbacks,
           struct diff_cmd_baton *callback_baton,
           apr_pool_t *pool)
  svn_error_t *err;
  svn_node_kind_t kind;
  SVN_ERR(svn_dirent_get_absolute(&abspath1, path1, pool));
  /* Resolve named revisions to real numbers. */
  err = svn_client__get_revision_number(&callback_baton->revnum1, NULL,
                                        ctx->wc_ctx, abspath1, NULL,
                                        revision1, pool);
  /* In case of an added node, we have no base rev, and we show a revision
   * number of 0. Note that this code is currently always asking for
   * svn_opt_revision_base.
   * ### TODO: get rid of this 0 for added nodes. */
  if (err && (err->apr_err == SVN_ERR_CLIENT_BAD_REVISION))
    {
      svn_error_clear(err);
      callback_baton->revnum1 = 0;
  else
    SVN_ERR(err);
  callback_baton->revnum2 = SVN_INVALID_REVNUM;  /* WC */

  SVN_ERR(svn_wc_read_kind2(&kind, ctx->wc_ctx, abspath1,
                            TRUE, FALSE, pool));

  if (kind != svn_node_dir)
    callback_baton->anchor = svn_dirent_dirname(path1, pool);
  else
    callback_baton->anchor = path1;

  SVN_ERR(svn_wc_diff6(ctx->wc_ctx,
                       abspath1,
                       callbacks, callback_baton,
                       depth,
                       ignore_ancestry, show_copies_as_adds,
                       use_git_diff_format, changelists,
                       ctx->cancel_func, ctx->cancel_baton,
                       pool));
diff_repos_repos(const svn_wc_diff_callbacks4_t *callbacks,
                 struct diff_cmd_baton *callback_baton,
                 svn_client_ctx_t *ctx,
                 apr_pool_t *pool)
  const svn_diff_tree_processor_t *diff_processor;

  const char *base_path;
  const char *wri_abspath = NULL;
  SVN_ERR(diff_prepare_repos_repos(&url1, &url2, &base_path, &rev1, &rev2,
                                   pool));

  /* Find a WC path for the ra session */
  if (!svn_path_is_url(path_or_url1))
    SVN_ERR(svn_dirent_get_absolute(&wri_abspath, path_or_url1, pool));
  else if (!svn_path_is_url(path_or_url2))
    SVN_ERR(svn_dirent_get_absolute(&wri_abspath, path_or_url2, pool));
  SVN_ERR(svn_wc__wrap_diff_callbacks(&diff_processor,
                                      callbacks, callback_baton,
                                      TRUE /* walk_deleted_dirs */,
                                      pool, pool));
  /* Get actual URLs. */
  callback_baton->orig_path_1 = url1;
  callback_baton->orig_path_2 = url2;

  /* Get numeric revisions. */
  callback_baton->revnum1 = rev1;
  callback_baton->revnum2 = rev2;

  callback_baton->ra_session = ra_session;
  callback_baton->anchor = base_path;
                                                               NULL, pool);
  if ((kind1 != svn_node_file && kind2 != svn_node_file) && target1[0] != '\0')
  {
    diff_processor = svn_diff__tree_processor_filter_create(diff_processor,
                                                            target1, pool);
  }
  SVN_ERR(svn_client_open_ra_session2(&extra_ra_session, anchor1, wri_abspath,
                                      ctx, pool, pool));
                TRUE /* text_deltas */,
                pool));
                          depth, ignore_ancestry, TRUE /* text_deltas */,
                          url2, diff_editor, diff_edit_baton, pool));
                             pool));
  return svn_error_trace(reporter->finish_report(reporter_baton, pool));
diff_repos_wc(const char *path_or_url1,
              svn_boolean_t show_copies_as_adds,
              svn_boolean_t use_git_diff_format,
              const svn_wc_diff_callbacks4_t *callbacks,
              void *callback_baton,
              struct diff_cmd_baton *cmd_baton,
  apr_pool_t *pool = scratch_pool;
  const char *url1, *anchor, *anchor_url, *target;
  svn_revnum_t rev;
  svn_node_kind_t kind1;
  svn_node_kind_t kind2;
      SVN_ERR(svn_dirent_get_absolute(&abspath_or_url1, path_or_url1, pool));
      SVN_ERR(svn_wc__node_get_url(&url1, ctx->wc_ctx, abspath_or_url1,
                                   pool, pool));
      url1 = path_or_url1;
  SVN_ERR(svn_dirent_get_absolute(&abspath2, path2, pool));
  /* Convert path_or_url1 to a URL to feed to do_diff. */
  SVN_ERR(svn_wc_get_actual_target2(&anchor, &target,
                                    ctx->wc_ctx, path2,
                                    pool, pool));
  /* Fetch the URL of the anchor directory. */
  SVN_ERR(svn_dirent_get_absolute(&anchor_abspath, anchor, pool));
  SVN_ERR(svn_wc__node_get_url(&anchor_url, ctx->wc_ctx, anchor_abspath,
                               pool, pool));
  SVN_ERR_ASSERT(anchor_url != NULL);
  /* If we are performing a pegged diff, we need to find out what our
     actual URLs will be. */
  if (peg_revision->kind != svn_opt_revision_unspecified)
      SVN_ERR(svn_client__repos_locations(&url1, NULL, NULL, NULL,
                                          NULL,
                                          path_or_url1,
                                          peg_revision,
                                          revision1, NULL,
                                          ctx, pool));
          cmd_baton->orig_path_1 = url1;
          cmd_baton->orig_path_2 =
            svn_path_url_add_component2(anchor_url, target, pool);
          cmd_baton->orig_path_1 =
            svn_path_url_add_component2(anchor_url, target, pool);
          cmd_baton->orig_path_2 = url1;
  /* Open an RA session to URL1 to figure out its node kind. */
  SVN_ERR(svn_client_open_ra_session2(&ra_session, url1, abspath2,
                                      ctx, pool, pool));
  /* Resolve the revision to use for URL1. */
  SVN_ERR(svn_client__get_revision_number(&rev, NULL, ctx->wc_ctx,
                                          (strcmp(path_or_url1, url1) == 0)
                                                    ? NULL : abspath_or_url1,
                                          ra_session, revision1, pool));
  SVN_ERR(svn_ra_check_path(ra_session, "", rev, &kind1, pool));

  /* Figure out the node kind of the local target. */
  SVN_ERR(svn_wc_read_kind2(&kind2, ctx->wc_ctx, abspath2,
                            TRUE, FALSE, pool));

  cmd_baton->ra_session = ra_session;
  cmd_baton->anchor = anchor;

  if (!reverse)
    cmd_baton->revnum1 = rev;
  else
    cmd_baton->revnum2 = rev;

  /* Check if our diff target is a copied node. */
  SVN_ERR(svn_wc__node_get_origin(&is_copy,
                                  &cf_revision,
                                  &cf_repos_relpath,
                                  &cf_repos_root_url,
                                  NULL, NULL,
                                  ctx->wc_ctx, abspath2,
                                  FALSE, pool, pool));
                                SVN_RA_CAPABILITY_DEPTH, pool));
                                  ignore_ancestry || is_copy,
                                  show_copies_as_adds,
                                  use_git_diff_format,
                                  callbacks, callback_baton,
                                  pool, pool));
  SVN_ERR(svn_ra_reparent(ra_session, anchor_url, pool));
  if (is_copy)
    {
      const char *copyfrom_parent_url;
      const char *copyfrom_basename;
      svn_depth_t copy_depth;
      cmd_baton->repos_wc_diff_target_is_copy = TRUE;

      /* We're diffing a locally copied/moved node.
       * Describe the copy source to the reporter instead of the copy itself.
       * Doing the latter would generate a single add_directory() call to the
       * diff editor which results in an unexpected diff (the copy would
       * be shown as deleted). */

      if (cf_repos_relpath[0] == '\0')
        {
          copyfrom_parent_url = cf_repos_root_url;
          copyfrom_basename = "";
        }
      else
        {
          const char *parent_relpath;
          svn_relpath_split(&parent_relpath, &copyfrom_basename,
                            cf_repos_relpath, scratch_pool);

          copyfrom_parent_url = svn_path_url_add_component2(cf_repos_root_url,
                                                            parent_relpath,
                                                            scratch_pool);
        }
      SVN_ERR(svn_ra_reparent(ra_session, copyfrom_parent_url, pool));
                              rev,
                              url1,
                              diff_editor, diff_edit_baton, pool));
      SVN_ERR(svn_wc__node_get_depth(&copy_depth, ctx->wc_ctx, abspath2,
                                     pool));

      if (copy_depth == svn_depth_unknown)
        copy_depth = svn_depth_infinity;
                                 copy_depth, FALSE, NULL, scratch_pool));
      if (strcmp(target, copyfrom_basename) != 0)
                                    svn_path_url_add_component2(
                                                cf_repos_root_url,
                                                cf_repos_relpath,
                                                scratch_pool),
                                    cf_revision,
                                    copy_depth, FALSE, NULL, scratch_pool));
      SVN_ERR(reporter->finish_report(reporter_baton, pool));
                              rev,
                              url1,
                              diff_editor, diff_edit_baton, pool));
                                      pool));
/* This is basically just the guts of svn_client_diff[_peg]6(). */
do_diff(const svn_wc_diff_callbacks4_t *callbacks,
        struct diff_cmd_baton *callback_baton,
        svn_client_ctx_t *ctx,
        svn_boolean_t show_copies_as_adds,
        svn_boolean_t use_git_diff_format,
        apr_pool_t *pool)
          SVN_ERR(diff_repos_repos(callbacks, callback_baton, ctx,
                                   pool));
          SVN_ERR(diff_repos_wc(path_or_url1, revision1, peg_revision,
                                ignore_ancestry, show_copies_as_adds,
                                use_git_diff_format, changelists,
                                callbacks, callback_baton, callback_baton,
                                ctx, pool));
          SVN_ERR(diff_repos_wc(path_or_url2, revision2, peg_revision,
                                ignore_ancestry, show_copies_as_adds,
                                use_git_diff_format, changelists,
                                callbacks, callback_baton, callback_baton,
                                ctx, pool));
              SVN_ERR(svn_dirent_get_absolute(&abspath1, path_or_url1, pool));
              SVN_ERR(svn_dirent_get_absolute(&abspath2, path_or_url2, pool));
              SVN_ERR(svn_client__arbitrary_nodes_diff(abspath1, abspath2,
                                                       callbacks,
                                                       callback_baton,
                                                       ctx, pool));
            SVN_ERR(diff_wc_wc(path_or_url1, revision1,
                               path_or_url2, revision2,
                               depth, ignore_ancestry, show_copies_as_adds,
                               use_git_diff_format, changelists,
                               callbacks, callback_baton, ctx, pool));
/* Perform a diff between a repository path and a working-copy path.

   PATH_OR_URL1 may be either a URL or a working copy path.  PATH2 is a
   working copy path.  REVISION1 and REVISION2 are their respective
   revisions.  If REVERSE is TRUE, the diff will be done in reverse.
   If PEG_REVISION is specified, then PATH_OR_URL1 is the path in the peg
   revision, and the actual repository path to be compared is
   determined by following copy history.

   All other options are the same as those passed to svn_client_diff6(). */
static svn_error_t *
diff_summarize_repos_wc(svn_client_diff_summarize_func_t summarize_func,
                        void *summarize_baton,
                        const char *path_or_url1,
                        const svn_opt_revision_t *revision1,
                        const svn_opt_revision_t *peg_revision,
                        const char *path2,
                        const svn_opt_revision_t *revision2,
                        svn_boolean_t reverse,
                        svn_depth_t depth,
                        svn_boolean_t ignore_ancestry,
                        const apr_array_header_t *changelists,
                        svn_client_ctx_t *ctx,
                        apr_pool_t *pool)
{
  const char *anchor, *target;
  svn_wc_diff_callbacks4_t *callbacks;
  void *callback_baton;
  struct diff_cmd_baton cmd_baton;

  SVN_ERR_ASSERT(! svn_path_is_url(path2));

  SVN_ERR(svn_wc_get_actual_target2(&anchor, &target,
                                    ctx->wc_ctx, path2,
                                    pool, pool));

  SVN_ERR(svn_client__get_diff_summarize_callbacks(
            &callbacks, &callback_baton, target, reverse,
            summarize_func, summarize_baton, pool));

  SVN_ERR(diff_repos_wc(path_or_url1, revision1, peg_revision,
                        path2, revision2, reverse,
                        depth, FALSE, TRUE, FALSE, changelists,
                        callbacks, callback_baton, &cmd_baton,
                        ctx, pool));
  return SVN_NO_ERROR;
}

/* Perform a summary diff between two working-copy paths.

   PATH1 and PATH2 are both working copy paths.  REVISION1 and
   REVISION2 are their respective revisions.

   All other options are the same as those passed to svn_client_diff6(). */
static svn_error_t *
diff_summarize_wc_wc(svn_client_diff_summarize_func_t summarize_func,
                     void *summarize_baton,
                     const char *path1,
                     const svn_opt_revision_t *revision1,
                     const char *path2,
                     const svn_opt_revision_t *revision2,
                     svn_depth_t depth,
                     svn_boolean_t ignore_ancestry,
                     const apr_array_header_t *changelists,
                     svn_client_ctx_t *ctx,
                     apr_pool_t *pool)
{
  svn_wc_diff_callbacks4_t *callbacks;
  void *callback_baton;
  const char *abspath1, *target1;
  svn_node_kind_t kind;

  SVN_ERR_ASSERT(! svn_path_is_url(path1));
  SVN_ERR_ASSERT(! svn_path_is_url(path2));

  /* Currently we support only the case where path1 and path2 are the
     same path. */
  if ((strcmp(path1, path2) != 0)
      || (! ((revision1->kind == svn_opt_revision_base)
             && (revision2->kind == svn_opt_revision_working))))
    return unsupported_diff_error
      (svn_error_create
       (SVN_ERR_INCORRECT_PARAMS, NULL,
        _("Summarized diffs are only supported between a path's text-base "
          "and its working files at this time")));

  /* Find the node kind of PATH1 so that we know whether the diff drive will
     be anchored at PATH1 or its parent dir. */
  SVN_ERR(svn_dirent_get_absolute(&abspath1, path1, pool));
  SVN_ERR(svn_wc_read_kind2(&kind, ctx->wc_ctx, abspath1,
                            TRUE, FALSE, pool));
  target1 = (kind == svn_node_dir) ? "" : svn_dirent_basename(path1, pool);
  SVN_ERR(svn_client__get_diff_summarize_callbacks(
            &callbacks, &callback_baton, target1, FALSE,
            summarize_func, summarize_baton, pool));

  SVN_ERR(svn_wc_diff6(ctx->wc_ctx,
                       abspath1,
                       callbacks, callback_baton,
                       depth,
                       ignore_ancestry, FALSE /* show_copies_as_adds */,
                       FALSE /* use_git_diff_format */, changelists,
                       ctx->cancel_func, ctx->cancel_baton,
                       pool));
  return SVN_NO_ERROR;
}

/* Perform a diff summary between two repository paths. */
static svn_error_t *
diff_summarize_repos_repos(svn_client_diff_summarize_func_t summarize_func,
                           void *summarize_baton,
                           svn_client_ctx_t *ctx,
                           const char *path_or_url1,
                           const char *path_or_url2,
                           const svn_opt_revision_t *revision1,
                           const svn_opt_revision_t *revision2,
                           const svn_opt_revision_t *peg_revision,
                           svn_depth_t depth,
                           svn_boolean_t ignore_ancestry,
                           apr_pool_t *pool)
{
  svn_ra_session_t *extra_ra_session;

  const svn_ra_reporter3_t *reporter;
  void *reporter_baton;

  const svn_delta_editor_t *diff_editor;
  void *diff_edit_baton;

  const svn_diff_tree_processor_t *diff_processor;

  const char *url1;
  const char *url2;
  const char *base_path;
  svn_revnum_t rev1;
  svn_revnum_t rev2;
  svn_node_kind_t kind1;
  svn_node_kind_t kind2;
  const char *anchor1;
  const char *anchor2;
  const char *target1;
  const char *target2;
  svn_ra_session_t *ra_session;
  svn_wc_diff_callbacks4_t *callbacks;
  void *callback_baton;

  /* Prepare info for the repos repos diff. */
  SVN_ERR(diff_prepare_repos_repos(&url1, &url2, &base_path, &rev1, &rev2,
                                   &anchor1, &anchor2, &target1, &target2,
                                   &kind1, &kind2, &ra_session,
                                   ctx, path_or_url1, path_or_url2,
                                   revision1, revision2,
                                   peg_revision, pool));

  /* Set up the repos_diff editor. */
  SVN_ERR(svn_client__get_diff_summarize_callbacks(
            &callbacks, &callback_baton,
            target1, FALSE, summarize_func, summarize_baton, pool));

  SVN_ERR(svn_wc__wrap_diff_callbacks(&diff_processor,
                                      callbacks, callback_baton,
                                      TRUE /* walk_deleted_dirs */,
                                      pool, pool));


  /* The repository can bring in a new working copy, but not delete
     everything. Luckily our new diff handler can just be reversed. */
  if (kind2 == svn_node_none)
    {
      const char *str_tmp;
      svn_revnum_t rev_tmp;

      str_tmp = url2;
      url2 = url1;
      url1 = str_tmp;

      rev_tmp = rev2;
      rev2 = rev1;
      rev1 = rev_tmp;

      str_tmp = anchor2;
      anchor2 = anchor1;
      anchor1 = str_tmp;

      str_tmp = target2;
      target2 = target1;
      target1 = str_tmp;

      diff_processor = svn_diff__tree_processor_reverse_create(diff_processor,
                                                               NULL, pool);
    }

  /* Now, we open an extra RA session to the correct anchor
     location for URL1.  This is used to get deleted path information.  */
  SVN_ERR(svn_client_open_ra_session2(&extra_ra_session, anchor1, NULL,
                                      ctx, pool, pool));

  SVN_ERR(svn_client__get_diff_editor2(&diff_editor, &diff_edit_baton,
                                       extra_ra_session,
                                       depth,
                                       rev1,
                                       FALSE /* text_deltas */,
                                       diff_processor,
                                       ctx->cancel_func, ctx->cancel_baton,
                                       pool));

  /* We want to switch our txn into URL2 */
  SVN_ERR(svn_ra_do_diff3
          (ra_session, &reporter, &reporter_baton, rev2, target1,
           depth, ignore_ancestry,
           FALSE /* do not create text delta */, url2, diff_editor,
           diff_edit_baton, pool));

  /* Drive the reporter; do the diff. */
  SVN_ERR(reporter->set_path(reporter_baton, "", rev1,
                             svn_depth_infinity,
                             FALSE, NULL, pool));
  return svn_error_trace(reporter->finish_report(reporter_baton, pool));
}

/* This is basically just the guts of svn_client_diff_summarize[_peg]2(). */
static svn_error_t *
do_diff_summarize(svn_client_diff_summarize_func_t summarize_func,
                  void *summarize_baton,
                  svn_client_ctx_t *ctx,
                  const char *path_or_url1,
                  const char *path_or_url2,
                  const svn_opt_revision_t *revision1,
                  const svn_opt_revision_t *revision2,
                  const svn_opt_revision_t *peg_revision,
                  svn_depth_t depth,
                  svn_boolean_t ignore_ancestry,
                  const apr_array_header_t *changelists,
                  apr_pool_t *pool)
{
  svn_boolean_t is_repos1;
  svn_boolean_t is_repos2;

  /* Check if paths/revisions are urls/local. */
  SVN_ERR(check_paths(&is_repos1, &is_repos2, path_or_url1, path_or_url2,
                      revision1, revision2, peg_revision));

  if (is_repos1)
    {
      if (is_repos2)
        SVN_ERR(diff_summarize_repos_repos(summarize_func, summarize_baton, ctx,
                                           path_or_url1, path_or_url2,
                                           revision1, revision2,
                                           peg_revision, depth, ignore_ancestry,
                                           pool));
      else
        SVN_ERR(diff_summarize_repos_wc(summarize_func, summarize_baton,
                                        path_or_url1, revision1,
                                        peg_revision,
                                        path_or_url2, revision2,
                                        FALSE, depth,
                                        ignore_ancestry,
                                        changelists,
                                        ctx, pool));
    }
  else /* ! is_repos1 */
    {
      if (is_repos2)
        SVN_ERR(diff_summarize_repos_wc(summarize_func, summarize_baton,
                                        path_or_url2, revision2,
                                        peg_revision,
                                        path_or_url1, revision1,
                                        TRUE, depth,
                                        ignore_ancestry,
                                        changelists,
                                        ctx, pool));
      else
        {
          if (revision1->kind == svn_opt_revision_working
              && revision2->kind == svn_opt_revision_working)
           {
             const char *abspath1;
             const char *abspath2;
             svn_wc_diff_callbacks4_t *callbacks;
             void *callback_baton;
             const char *target;
             svn_node_kind_t kind;

             SVN_ERR(svn_dirent_get_absolute(&abspath1, path_or_url1, pool));
             SVN_ERR(svn_dirent_get_absolute(&abspath2, path_or_url2, pool));

             SVN_ERR(svn_io_check_resolved_path(abspath1, &kind, pool));

             if (kind == svn_node_dir)
               target = "";
             else
               target = svn_dirent_basename(path_or_url1, NULL);

             SVN_ERR(svn_client__get_diff_summarize_callbacks(
                     &callbacks, &callback_baton, target, FALSE,
                     summarize_func, summarize_baton, pool));

             SVN_ERR(svn_client__arbitrary_nodes_diff(abspath1, abspath2,
                                                      depth,
                                                      callbacks,
                                                      callback_baton,
                                                      ctx, pool));
           }
          else
            SVN_ERR(diff_summarize_wc_wc(summarize_func, summarize_baton,
                                         path_or_url1, revision1,
                                         path_or_url2, revision2,
                                         depth, ignore_ancestry,
                                         changelists, ctx, pool));
      }
    }

  return SVN_NO_ERROR;
}


/* Initialize DIFF_CMD_BATON.diff_cmd and DIFF_CMD_BATON.options,
 * Allocate the fields in POOL, which should be at least as long-lived
 * as the pool DIFF_CMD_BATON itself is allocated in.
set_up_diff_cmd_and_options(struct diff_cmd_baton *diff_cmd_baton,
                            const apr_array_header_t *options,
                            apr_hash_t *config, apr_pool_t *pool)
            options = svn_cstring_split(diff_extensions, " \t\n\r", TRUE, pool);
    options = apr_array_make(pool, 0, sizeof(const char *));
    SVN_ERR(svn_path_cstring_to_utf8(&diff_cmd_baton->diff_cmd, diff_cmd,
                                     pool));
    diff_cmd_baton->diff_cmd = NULL;
  if (diff_cmd_baton->diff_cmd)
          argv = apr_palloc(pool, argc * sizeof(char *));
                      APR_ARRAY_IDX(options, i, const char *), pool));
      diff_cmd_baton->options.for_external.argv = argv;
      diff_cmd_baton->options.for_external.argc = argc;
      diff_cmd_baton->options.for_internal
        = svn_diff_file_options_create(pool);
      SVN_ERR(svn_diff_file_options_parse
              (diff_cmd_baton->options.for_internal, options, pool));
  struct diff_cmd_baton diff_cmd_baton = { 0 };
  diff_cmd_baton.orig_path_1 = path_or_url1;
  diff_cmd_baton.orig_path_2 = path_or_url2;

  SVN_ERR(set_up_diff_cmd_and_options(&diff_cmd_baton, options,
                                      ctx->config, pool));
  diff_cmd_baton.pool = pool;
  diff_cmd_baton.outstream = outstream;
  diff_cmd_baton.errstream = errstream;
  diff_cmd_baton.header_encoding = header_encoding;
  diff_cmd_baton.revnum1 = SVN_INVALID_REVNUM;
  diff_cmd_baton.revnum2 = SVN_INVALID_REVNUM;

  diff_cmd_baton.force_binary = ignore_content_type;
  diff_cmd_baton.ignore_properties = ignore_properties;
  diff_cmd_baton.properties_only = properties_only;
  diff_cmd_baton.relative_to_dir = relative_to_dir;
  diff_cmd_baton.use_git_diff_format = use_git_diff_format;
  diff_cmd_baton.no_diff_added = no_diff_added;
  diff_cmd_baton.no_diff_deleted = no_diff_deleted;
  diff_cmd_baton.no_copyfrom_on_add = show_copies_as_adds;

  diff_cmd_baton.wc_ctx = ctx->wc_ctx;
  diff_cmd_baton.ra_session = NULL;
  diff_cmd_baton.anchor = NULL;

  return do_diff(&diff_callbacks, &diff_cmd_baton, ctx,
                 path_or_url1, path_or_url2, revision1, revision2,
                 &peg_revision,
                 depth, ignore_ancestry, show_copies_as_adds,
                 use_git_diff_format, changelists, pool);
  struct diff_cmd_baton diff_cmd_baton = { 0 };
  diff_cmd_baton.orig_path_1 = path_or_url;
  diff_cmd_baton.orig_path_2 = path_or_url;

  SVN_ERR(set_up_diff_cmd_and_options(&diff_cmd_baton, options,
                                      ctx->config, pool));
  diff_cmd_baton.pool = pool;
  diff_cmd_baton.outstream = outstream;
  diff_cmd_baton.errstream = errstream;
  diff_cmd_baton.header_encoding = header_encoding;
  diff_cmd_baton.revnum1 = SVN_INVALID_REVNUM;
  diff_cmd_baton.revnum2 = SVN_INVALID_REVNUM;

  diff_cmd_baton.force_binary = ignore_content_type;
  diff_cmd_baton.ignore_properties = ignore_properties;
  diff_cmd_baton.properties_only = properties_only;
  diff_cmd_baton.relative_to_dir = relative_to_dir;
  diff_cmd_baton.use_git_diff_format = use_git_diff_format;
  diff_cmd_baton.no_diff_added = no_diff_added;
  diff_cmd_baton.no_diff_deleted = no_diff_deleted;
  diff_cmd_baton.no_copyfrom_on_add = show_copies_as_adds;

  diff_cmd_baton.wc_ctx = ctx->wc_ctx;
  diff_cmd_baton.ra_session = NULL;
  diff_cmd_baton.anchor = NULL;

  return do_diff(&diff_callbacks, &diff_cmd_baton, ctx,
                 path_or_url, path_or_url, start_revision, end_revision,
                 peg_revision,
                 depth, ignore_ancestry, show_copies_as_adds,
                 use_git_diff_format, changelists, pool);
  svn_opt_revision_t peg_revision;
  return do_diff_summarize(summarize_func, summarize_baton, ctx,
                           path_or_url1, path_or_url2, revision1, revision2,
                           &peg_revision,
                           depth, ignore_ancestry, changelists, pool);
  return do_diff_summarize(summarize_func, summarize_baton, ctx,
                           path_or_url, path_or_url,
                           start_revision, end_revision, peg_revision,
                           depth, ignore_ancestry, changelists, pool);
svn_client_diff_summarize_t *
svn_client_diff_summarize_dup(const svn_client_diff_summarize_t *diff,
                              apr_pool_t *pool)
{
  svn_client_diff_summarize_t *dup_diff = apr_palloc(pool, sizeof(*dup_diff));

  *dup_diff = *diff;

  if (diff->path)
    dup_diff->path = apr_pstrdup(pool, diff->path);

  return dup_diff;
}