
#
# Simple script to launch all the examples
#



for folder in *; do
    if [ -d "${folder}" ]; then

		checkscript="${folder}/check"


		if [ -f "$checkscript" ];
		then
			cd $folder
			echo
			echo "#################### Folder $folder  ####################"
			echo
			./check all
			cd ..
		fi

    fi
done
