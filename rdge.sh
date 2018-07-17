alias rdge='cd /Users/jbramlett/Documents/Projects/RDGE'
alias cmakeg='cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../'
alias cmakegr='cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../'

alias debm='make -j4 -C/Users/jbramlett/Documents/Projects/RDGE/debug'

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
