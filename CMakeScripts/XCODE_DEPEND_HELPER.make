# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.main.Debug:
PostBuild.src.Debug: /Users/chunyangmou/git_repos/pa6-mouchunyang/Debug/main
/Users/chunyangmou/git_repos/pa6-mouchunyang/Debug/main:\
	/Users/chunyangmou/git_repos/pa6-mouchunyang/src/Debug/libsrc.a
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/Debug/main


PostBuild.src.Debug:
/Users/chunyangmou/git_repos/pa6-mouchunyang/src/Debug/libsrc.a:
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/src/Debug/libsrc.a


PostBuild.tests.Debug:
PostBuild.src.Debug: /Users/chunyangmou/git_repos/pa6-mouchunyang/tests/Debug/tests
/Users/chunyangmou/git_repos/pa6-mouchunyang/tests/Debug/tests:\
	/Users/chunyangmou/git_repos/pa6-mouchunyang/src/Debug/libsrc.a
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/tests/Debug/tests


PostBuild.main.Release:
PostBuild.src.Release: /Users/chunyangmou/git_repos/pa6-mouchunyang/Release/main
/Users/chunyangmou/git_repos/pa6-mouchunyang/Release/main:\
	/Users/chunyangmou/git_repos/pa6-mouchunyang/src/Release/libsrc.a
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/Release/main


PostBuild.src.Release:
/Users/chunyangmou/git_repos/pa6-mouchunyang/src/Release/libsrc.a:
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/src/Release/libsrc.a


PostBuild.tests.Release:
PostBuild.src.Release: /Users/chunyangmou/git_repos/pa6-mouchunyang/tests/Release/tests
/Users/chunyangmou/git_repos/pa6-mouchunyang/tests/Release/tests:\
	/Users/chunyangmou/git_repos/pa6-mouchunyang/src/Release/libsrc.a
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/tests/Release/tests


PostBuild.main.MinSizeRel:
PostBuild.src.MinSizeRel: /Users/chunyangmou/git_repos/pa6-mouchunyang/MinSizeRel/main
/Users/chunyangmou/git_repos/pa6-mouchunyang/MinSizeRel/main:\
	/Users/chunyangmou/git_repos/pa6-mouchunyang/src/MinSizeRel/libsrc.a
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/MinSizeRel/main


PostBuild.src.MinSizeRel:
/Users/chunyangmou/git_repos/pa6-mouchunyang/src/MinSizeRel/libsrc.a:
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/src/MinSizeRel/libsrc.a


PostBuild.tests.MinSizeRel:
PostBuild.src.MinSizeRel: /Users/chunyangmou/git_repos/pa6-mouchunyang/tests/MinSizeRel/tests
/Users/chunyangmou/git_repos/pa6-mouchunyang/tests/MinSizeRel/tests:\
	/Users/chunyangmou/git_repos/pa6-mouchunyang/src/MinSizeRel/libsrc.a
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/tests/MinSizeRel/tests


PostBuild.main.RelWithDebInfo:
PostBuild.src.RelWithDebInfo: /Users/chunyangmou/git_repos/pa6-mouchunyang/RelWithDebInfo/main
/Users/chunyangmou/git_repos/pa6-mouchunyang/RelWithDebInfo/main:\
	/Users/chunyangmou/git_repos/pa6-mouchunyang/src/RelWithDebInfo/libsrc.a
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/RelWithDebInfo/main


PostBuild.src.RelWithDebInfo:
/Users/chunyangmou/git_repos/pa6-mouchunyang/src/RelWithDebInfo/libsrc.a:
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/src/RelWithDebInfo/libsrc.a


PostBuild.tests.RelWithDebInfo:
PostBuild.src.RelWithDebInfo: /Users/chunyangmou/git_repos/pa6-mouchunyang/tests/RelWithDebInfo/tests
/Users/chunyangmou/git_repos/pa6-mouchunyang/tests/RelWithDebInfo/tests:\
	/Users/chunyangmou/git_repos/pa6-mouchunyang/src/RelWithDebInfo/libsrc.a
	/bin/rm -f /Users/chunyangmou/git_repos/pa6-mouchunyang/tests/RelWithDebInfo/tests




# For each target create a dummy ruleso the target does not have to exist
/Users/chunyangmou/git_repos/pa6-mouchunyang/src/Debug/libsrc.a:
/Users/chunyangmou/git_repos/pa6-mouchunyang/src/MinSizeRel/libsrc.a:
/Users/chunyangmou/git_repos/pa6-mouchunyang/src/RelWithDebInfo/libsrc.a:
/Users/chunyangmou/git_repos/pa6-mouchunyang/src/Release/libsrc.a:
