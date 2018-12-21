alias rdge='cd /Users/jbramlett/Documents/Projects/RDGE'
alias cmakeg='cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=/Users/jbramlett/Documents/GitHub/vcpkg/scripts/buildsystems/vcpkg.cmake ../'
alias cmakegr='cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/Users/jbramlett/Documents/GitHub/vcpkg/scripts/buildsystems/vcpkg.cmake ../'

# The redirect gets rid of the annoying ld warnings on the mac
# https://stackoverflow.com/a/15936384
alias debm='make -j4 -C/Users/jbramlett/Documents/Projects/RDGE/debug 2> >(grep -v "text-based stub file" 1>&2)'

function chronopack()
{
    cd ~/Documents/Projects/RDGE/scripts/cooker/
    python export_all.py -f ../../sandbox/chrono/res/export.json
    if [ $? -eq 0 ]
    then
        cd -
        cd ~/Documents/Projects/RDGE/debug/tools/asset_packer/
        ./asset_packer ../../../sandbox/chrono/res/export.json --silent
    fi
    cd -
}
