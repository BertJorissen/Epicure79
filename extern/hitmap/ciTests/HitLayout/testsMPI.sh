#!/bin/bash

STD_PROCS="1 4 5 18"
YELLOW="\033[1;33m"
RESET_COLOR="\033[0m"
UNDERLINE="\e[4m"
RESET_UNDERLINE="\e[0m"
HOR="\\u2500"

PROCS=1
file=''
debug=0
verbose=0
passed=0
failed=0
ran=0
files=0
error=0


compile() {
   f=${1:-""}

   if [ "$f" != "" ]; then
      if [ $debug == 1 ]; then 
         make debugmode "$f" > /dev/null 2>>testserrors.txt
      else
         make clean > /dev/null
         make "$f" > /dev/null 2>>testserrors.txt
      fi
   else
      if [ $debug == 1 ]; then 
         make debugmode > /dev/null 2>>testserrors.txt
      else
         make clean > /dev/null
         make > /dev/null 2>>testserrors.txt
      fi
   fi

   if [ $? != 0 ]; then
      exit 1
   fi
}

execute() {
   exefile=$1
   sourcefile=${2:-""}

   if [ "$sourcefile" == "" ]; then
      noextension=$(echo $exefile | cut -d. -f1)
      sourcefile="$noextension""Test.c"

      if [ ! -f "$sourcefile" ]; then
         echo -e "$exefile: no such file $cfile"
         echo
         exit 1
      fi
   fi

   PROCS=$(getNProcs "$sourcefile")
   tmpfile=$(mktemp)
   let files=$files+1
   for numprocs in $PROCS; do
      
      errors=''
      let ran=$ran+1
      if [ $verbose == 0 ]; then
         echo "$exefile  $numprocs " >> testsoutput.txt
      else
         echo -e "$YELLOW""$exefile"  $numprocs "$RESET_COLOR"
      fi

      mpiexec -n $numprocs ./$exefile > ${tmpfile} 2>>testserrors.txt

      if [ $? != 0 ]; then
         echo 'ERROR. Check for details in file testserrors.txt'
         exit 1
      fi

      errors=$(grep ERROR "$tmpfile")

      if [ "$errors" != "" ]; then 
         let failed=$failed+1
         error=1
      else
         let passed=$passed+1
      fi

      if [ $verbose == 0 ]; then
         cat "$tmpfile" >> testsoutput.txt
         echo >> testsoutput.txt
      else
         cat "$tmpfile"
         echo
      fi

   done

   rm ${tmpfile}

}

getNProcs() {
   file=$1
   ok=$(grep \#nprocs "$file")
   if [[ "$ok" == "" ]]; then
      PROCS="$STD_PROCS"
   else
      PROCS=$(echo "$ok" | cut -d: -f2-)
   fi
   echo "$PROCS"
}

printUsage() {
   echo "Usage: $0 [-d] [-v] [.c file]"
}

printResultTable() {
   celllines="\u2500\\u2500\\u2500\\u2500\\u2500\\u2500\\u2500"

   printf "\\u250C"
   #
   for a in 1 2 3; do
      for n in 1 2  ; do printf "\u2500\\u2500\\u2500\\u2500\\u2500\\u2500\\u2500"; done
      printf "\\u2500"
      #
      printf "\\u252C"
      #
   done
   #
   for n in 1 2; do printf "$celllines"; done
   printf "\\u2500"
   #
   printf "\\u2510"
   printf "\n"
   ## end first line
   printf "\\u2502  %s\t\\u2502  %s\t\\u2502  %s\t\\u2502  %s\t\\u2502\n" "Files tested" "Tests ran" "Passed" "Failed" 
   #
   # begin second drawed line
   printf "\\u251C"
   #
   for a in 1 2 3; do
      for n in 1 2 ; do printf "$celllines"; done
      printf "\\u2500"
      #
      printf "\\u253C"
      #
   done
   #
   for n in 1 2  ; do printf "$celllines"; done
   printf "\\u2500"
   #
   printf "\\u2524"
   printf "\n"
   # end second line
   printf "\\u2502\t%d\t\\u2502\t%d\t\\u2502\t%d\t\\u2502\t%d\t\\u2502\n" "$files" "$ran" "$passed" "$failed"
   #
   printf "\\u2514"
   #
   for a in 1 2 3; do
      for n in 1 2  ; do printf "$celllines"; done
      printf "\\u2500"
      #
      printf "\\u2534"
      #
   done
   #
   for n in 1 2  ; do printf "$celllines"; done
   printf "\\u2500"
   #
   printf "\\u2518"
   printf "\n"   
}

printResults() {
   echo -e "$YELLOW$UNDERLINE RESULTS $RESET_UNDERLINE$RESET_COLOR"
   echo 'Files tested:' $files
   echo 'Tests ran:'    $ran
   echo 'Passed:' $passed
   echo 'Failed:' $failed
   echo

   if [ $verbose == 0 ]; then
      echo 'Complete output in file testsoutput.txt'
   fi
   echo 'Errors and warnings in file testserrors.txt'
}


# Parse command line arguments
if [ $# -gt 3 ]; then
   printUsage
   exit 1
   
else
   for i in "$@"; do
      case $i in
         -d)
            debug=1
            shift
            ;;

         -v)
            verbose=1
            shift
            ;;
         
         *)
            if [ "$file" != "" ]; then
               printUsage
               exit 1
            fi

            file="$i"
            ;;
      esac
   done

fi

#Output files
echo > testserrors.txt
if [ $verbose == 0 ]; then echo > testsoutput.txt; fi


# Check if file exists
if [ "$file" != "" ] && [ ! -f "$file" ]; then
   echo -e "ERROR: No such file $file"
   exit 1
fi


## 1 FILE
if [ "$file" != "" ]; then
   testFile=${file::-6}    # Remove 'Test.c'
   testFile="$testFile.test"

   # COMPILE
   compile "$testFile"

   if [ $? != 0 ]; then
      echo 'ERROR. Check for details in file testserrors.txt'
      exit 1
   fi

   
   # EXECUTE
   execute "$testFile" "$file"


   # GLOBAL RESULTS
   if [ $verbose == 1 ]; then echo; echo; fi
   # printResultTable
   printResults



## ALL FILES
else

   # COMPILE
   compile 

   if [ $? != 0 ]; then
      echo 'ERROR. Check for details in file testserrors.txt'
      exit 1
   fi

   # EXECUTE
   t=$(ls *.test)

   for testFile in $t; do

      execute "$testFile"


      if [ $verbose == 0 ]; then
         echo '--------------------------------------------------' >> testsoutput.txt
      else
         echo '--------------------------------------------------'
      fi

   done

   printResults

fi

exit $error


