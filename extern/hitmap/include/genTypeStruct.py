#!/usr/bin/python

#
# Script to automatically generate macros to define new Hitmap communication
#	derived data types for structs, with as many fields as needed.
#
# The result of this script is stored in: hit_comTypeStruct.h
#
# @version 1.0
# @author Javier Fresno Bausela
# @author Arturo Gonzalez-Escribano
# @date Mar 2013
# @date Jun 2013 Changed loop variables for something longer to avoid warnings.
# @date Ago 2015 Add the generation of the license tags
#
#
# This software is provided to enhance knowledge and encourage progress
# in the scientific community and are to be used only for research and
# educational purposes. Any reproduction or use for commercial purpose
# is prohibited without the prior express written permission.
#
# This software is provided "as is" and without any express or implied
# warranties, including, without limitation, the implied warranties of
# merchantability and fitness for a particular purpose.
#
# Copyright (c) 2013, Javier Fresno Bausela
# All Rights Reserved.
#


# Imports
import sys
import string
import itertools

# Config

# Number of generated macros, we are using the A,B,...,Z,AA,..ZY,ZZ
# names for the variables so the current limit is 26 + 26 * 26 = 702 macros.
NUM_MACROS = 100 


##############################################################

# Generate the letters
one_letter_items = list(string.uppercase)
two_letter_items = [''.join(x) for x in
    itertools.product(string.uppercase, repeat=2)]
letters = one_letter_items + two_letter_items

# Print a comment
def comment(text):
	sys.stdout.write("// ")
	sys.stdout.write(text)
	sys.stdout.write("\n")

# Print a multiline comment
def big_comment(text):
	sys.stdout.write("/**\n * ")
	text = text.replace("\n","\n * ")
	sys.stdout.write(text)
	sys.stdout.write(" \n */\n")

# Print a line with a tab and the continue sysmbol
def macro_line(text):
	sys.stdout.write("\t" + text + " \\\n")
   
# Prints a struct macro for the given number of variables
def print_struct_macro(nvars):

	comment("Macro for " + str(nvars) + " struct elements" )

	# Macro header
	sys.stdout.write("#define hit_comTypeStruct"+str(nvars))
	sys.stdout.write("(new_type, NATIVE_STRUCT")
	for i in range(nvars):
		sys.stdout.write(", NAME_"+letters[i]+", COUNT_"+letters[i]+", TYPE_"+letters[i])
	sys.stdout.write(") \\\n")
	sys.stdout.write("{ \\\n")

	# Native struct
	macro_line("NATIVE_STRUCT cmdline;")

	# Block counts
	sys.stdout.write("\tint blockcounts[" + str(nvars) + "] = {(COUNT_A)")
	for i in range(1,nvars):
		sys.stdout.write(", (COUNT_"+letters[i]+")")
	sys.stdout.write("}; \\\n")

	# Datatypes
	sys.stdout.write("\tMPI_Datatype types[" + str(nvars) + "] = {(TYPE_A)")
	for i in range(1,nvars):
		sys.stdout.write(", (TYPE_"+letters[i]+")")
	sys.stdout.write("}; \\\n")

	# Displacements
	macro_line("MPI_Aint displs[" + str(nvars) + "];")
	for i in range(nvars):
		macro_line("MPI_Get_address(&cmdline.NAME_" +letters[i] + ", &displs["+ str(i) +"]);")

	# Loop for fix displacements
	macro_line("int ivarloop;")
	macro_line("for(ivarloop=" + str(nvars-1) + "; ivarloop>=0; ivarloop--){")
	macro_line("\tdispls[ivarloop] -= displs[0];")
	macro_line("}")
	
	# MPI functions
	macro_line("MPI_Type_create_struct(" + str(nvars) + ", blockcounts, displs, types, new_type);")
	macro_line("MPI_Type_commit(new_type);")

	sys.stdout.write("} \n")
	print

##############################################################

# Change the output
sys.stdout = open('hit_comTypeStruct.h', 'w')

# Comments
big_comment("Macros to create the MPI Struct Datatypes.\n"
"The script to generate this file is included at the end.\n\n"
"<license>\n"
"</license>\n")
print

for i in range(1,NUM_MACROS+1):
	print_struct_macro(i)	

# Inlude the file in the .h
big_comment("Python script to generate this file")
print "#ifdef NOT_INCLUDE_THIS_BEACUSE_IS_THE_PYTHON_SCRIPT"
f = open(sys.argv[0])
lines = f.readlines()
for l in lines:
	sys.stdout.write(l)
f.close()
print "#endif"
print




