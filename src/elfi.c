#include <stdio.h>
#include <elfi.h>
#include <opt.h>
#include <utility/utils.h>


// AVAILABLE OPTS
option_s OPT[] = {
	{'a', "--arch"           , "select arch, default 'x86_64'"                                      , OPT_STR  , 0, 0}, 
	{'m', "--mirrorfile"     , "use mirror file instead of downloading mirrorlist"                  , OPT_STR  , 0, 0},
	{'c', "--country"        , "select country from mirrorlist"                                     , OPT_ARRAY | OPT_STR  , 0, 0},
	{'C', "--country-list"   , "show all possibile country"                                         , OPT_NOARG, 0, 0},
	{'u', "--uncommented"    , "use only uncommented mirror"                                        , OPT_NOARG, 0, 0},
	{'d', "--downloads"      , "set numbers of parallel download"                                   , OPT_NUM  , 0, 0},
	{'O', "--timeout"        , "set timeout in seconds for not reply mirror"                        , OPT_NUM  , 0, 0},
	{'p', "--progress"       , "show progress, default false"                                       , OPT_NOARG, 0, 0},
	{'P', "--progress-colors", "same -p but with colors"                                            , OPT_NOARG, 0, 0},
	{'o', "--output"         , "enable table output"                                                , OPT_NOARG, 0, 0},
	{'s', "--speed"          , "test speed for downloading pkg, light/normal/heavy"                 , OPT_STR  , 0, 0},
	{'S', "--sort"           , "sort result for any of fields, mutiple fields supported"            , OPT_ARRAY | OPT_STR, 0, 0},
	{'l', "--list"           , "create a file with list of mirrors, stdout as arg for output here"  , OPT_STR, 0, 0},
	{'L', "--max-list"       , "set max numbers of output mirrors"                                  , OPT_NUM, 0, 0},
	{'T', "--type"           , "select mirrors type, http,https,all"                                , OPT_ARRAY | OPT_STR, 0, 0},
	{'i', "--investigate"    , "investigate on mirror, mode: outofdate/error/all"                   , OPT_ARRAY | OPT_STR, 0, 0},
	{'D', "--systemd"        , "auto manager systemd.timer"                                         , OPT_NOARG, 0, 0},
	{'t', "--time"           , "specific your preferite time in hh:mm:ss"                           , OPT_STR, 0, 0},
	{'f', "--fixed-time"     , "use fixed OnCalenda, systemd-analyzer calendar ..., for test valid" , OPT_STR, 0, 0},
	{'h', "--help"           , "display this"                                                       , OPT_END | OPT_NOARG, 0, 0}
};

int main(int argc, char **argv) {
  __argv option_s* opt = argv_parse(OPT, argc, argv); // NOLINT
	if( opt[O_h].set ) argv_usage(opt, argv[0]);
  print_args(opt);
  return 0;
}
