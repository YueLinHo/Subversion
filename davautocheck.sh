#!/bin/sh
# -*- mode: shell-script; -*-
# $Id$
SCRIPT=$(basename $0)

function say() {
  echo "$SCRIPT: $*"
}

function fail() {
  say $*
  exit 1
}

function query() {
  echo -n "$SCRIPT: $1 (y/n)? [$2] "
  read -n 1 -t 32
  echo
  [ "${REPLY:-$2}" == 'y' ]
}

function get_loadmodule_config() {
  local SO="$HTTPD_REAL_ROOT/libexec/$1.so"

  # shared object module?
  if [ -r "$SO" ]; then
    local NM=$(echo "$1" | sed 's|mod_\(.*\)|\1_module|')
    echo "LoadModule $NM \"$SO\"" &&
    return
  fi

  # maybe it's built-in?
  "$HTTPD" -l | grep -q "$1\\.c" && return

  return 1
}

pushd $(dirname $0) >/dev/null
ABS_BUILDDIR=$(pwd)
popd >/dev/null

[ -x "$ABS_BUILDDIR/configure" ] \
  || fail "can't operate if not located in the root of Subversion's tree"

MOD_DAV_SVN="$ABS_BUILDDIR/subversion/mod_dav_svn/.libs/mod_dav_svn.so"

[ -r "$MOD_DAV_SVN" ] \
  || fail "dav_svn_module not found, please use '--enable-shared --enable-dso --with-apxs' with your 'configure' script"

export LD_LIBRARY_PATH="$ABS_BUILDDIR/subversion/libsvn_ra_dav/.libs:$ABS_BUILDDIR/subversion/libsvn_ra_local/.libs:$ABS_BUILDDIR/subversion/libsvn_ra_svn/.libs"

CLIENT_CMD="$ABS_BUILDDIR/subversion/clients/cmdline/svn"
ldd "$CLIENT_CMD" | grep -q 'not found' \
  && fail "Subversion client couldn't be fully linked at run-time"
"$CLIENT_CMD" --version | grep -q '^[*] ra_dav' \
  || fail "Subversion client couldn't find and/or load ra_dav library"

HTTPD=$(which httpd 2>/dev/null) \
  || fail "HTTPD executable not found"

"$HTTPD" -v 1>/dev/null 2>&1 \
  || fail "HTTPD doesn't start properly"

say "Using '$HTTPD'..."

# need to remove quotes from whatevere 'cut' extracts
root=$("$HTTPD" -V | grep HTTPD_ROOT | cut -d '=' -f 2)
HTTPD_REAL_ROOT=${root//\"/}
[ -d "$HTTPD_REAL_ROOT" ] \
  || fail "HTTPD real root not found"

LOAD_MOD_DAV=$(get_loadmodule_config mod_dav) \
  || fail "DAV module not found"

LOAD_MOD_LOG_CONFIG=$(get_loadmodule_config mod_log_config) \
  || fail "log_config module not found"

HTTPD_PORT=$(($RANDOM+1024))
HTTPD_ROOT="$ABS_BUILDDIR/httpd-$(date '+%Y%m%d-%H%M%S')"
HTTPD_CFG="$HTTPD_ROOT/cfg"
HTTPD_PID="$HTTPD_ROOT/pid"
HTTPD_LOG="$HTTPD_ROOT/log"
BASE_URL="http://localhost:$HTTPD_PORT"

mkdir "$HTTPD_ROOT" \
  || fail "couldn't create temporary directory '$HTTPD_ROOT'"

say "Using directory '$HTTPD_ROOT'..."

cat > "$HTTPD_CFG" <<__EOF__
User                $(whoami)
Group               $(whoami)
Listen              localhost:$HTTPD_PORT
ServerName          localhost
PidFile             "$HTTPD_PID"
ErrorLog            "$HTTPD_LOG"
LogLevel            Debug
ServerRoot          "$HTTPD_ROOT"
DocumentRoot        "$HTTPD_ROOT"
ScoreBoardFile      "$HTTPD_ROOT/run"
CoreDumpDirectory   "$HTTPD_ROOT"
StartServers        4
MaxRequestsPerChild 0
<IfModule worker.c>
  ThreadsPerChild   8
</IfModule>
MaxClients          16
HostNameLookups     Off
$LOAD_MOD_DAV
LoadModule          dav_svn_module "$MOD_DAV_SVN"
$LOAD_MOD_LOG_CONFIG
LogFormat           "%h %l %u %t \"%r\" %>s %b \"%{Referer}i\" \"%{User-Agent}i\"" format
CustomLog           "$HTTPD_ROOT/req" format

<Directory />
  AllowOverride     none
</Directory>
<Directory "$HTTPD_ROOT">
  AllowOverride     none
</Directory>

<Location /repositories>
  DAV               svn
  SVNParentPath     "$ABS_BUILDDIR/subversion/tests/clients/cmdline/repositories"
</Location>
<Location /local_tmp/repos>
  DAV               svn
  SVNPath           "$ABS_BUILDDIR/subversion/tests/clients/cmdline/local_tmp/repos"
</Location>
__EOF__

START="$HTTPD -f $HTTPD_CFG"

$START -t \
  || fail "Configuration file didn't pass the check, most likely modules couldn't be loaded"

# need to pause for some time to let HTTPD start
$START &
sleep 2

say "HTTPD started and listening on '$BASE_URL'..."

# use wget to download configuration file through HTTPD and compare it to the original
wget -q -O "$HTTPD_CFG-copy" "$BASE_URL/cfg"
diff -q "$HTTPD_CFG" "$HTTPD_CFG-copy" \
  || fail "HTTPD doesn't operate according to the configuration"
rm "$HTTPD_CFG-copy"

say "HTTPD is good, starting the tests..."

if [ $# == 0 ]; then
  time make check "BASE_URL=$BASE_URL"
else
  pushd "$ABS_BUILDDIR/subversion/tests/clients/cmdline/" >/dev/null
  TEST="$1"
  shift
  time "./${TEST}_tests.py" "--url=$BASE_URL" $*
  popd >/dev/null
fi

say "Finished testing..."

kill $(cat "$HTTPD_PID")

query 'Browse server error log' n \
  && less "$HTTPD_LOG"

query 'Delete HTTPD root directory' y \
  && rm -fr "$HTTPD_ROOT/"

say 'Done'
