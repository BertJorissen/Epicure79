#!/bin/bash
#
# Generic stencil skeleton
#
# Generator of stencil kernels from stencil pattern specifications
#
# Author: Arturo Gonzalez-Escribano, 2021
# Version: 1.0
#

# 1. CHECK ARGUMENT
if [ $# -lt 1 ]; then
	echo "Usage: $0 <stencil_description_file.c>"
	exit -1
fi
if [ ! -r $1 ]; then
	echo "Error -- File does not exist or cannot be open: $1"
	echo "Usage: $0 <stencil_description_file.c>"
	exit -1
fi

# 2. PROCESS FILE
awk -e '
	BEGIN { 
		# print "Starting processing file"; 
		shp=0; 
		patt=0;
		patternVar = "";
	}

	/^[[:space:]]*HitShape/ {
		gsub( "^[[:space:]]*HitShape[[:space:]]+shp_", "" );
		line = $0;
		gsub( "[[:space:]]*=.*", "", line );
		varName = line;
		# print "Processing shape of var: " varName;

		line = $0;
		gsub( ".*=[[:space:]]*hitShape", "", line );
		gsub( "[[:space:]]*;[[:space:]]*$", "", line );
		gsub( "[[:space:]]", "", line );
		shapes[ varName ] = line;

		gsub( "^[(]", "", line );
		gsub( "[)]$", "", line );
		gsub( "^[(]", "", line );
		gsub( "[)]$", "", line );
		shapesDims[ varName ] = split( line, dimensions, "[)],[(]" );

		for( dim in dimensions ) {
			rangeNums = split( dimensions[dim], range, "," );
			if ( rangeNums != 2 ) {
				print "Error -- Incorrect range in dimension " dim-1 ": " dimensions[dim];
			}
			rangeBegins[ varName, dim-1 ] = range[1];
			rangeEnds[ varName, dim-1 ] = range[2];
		}
		shp++;
		next;
	}

	/^[[:space:]]*float[[:space:]]+factor_/ {
		gsub( "^[[:space:]]*float[[:space:]]+factor_", "" );
		line = $0;

		gsub( "[[:space:]]*=.*", "", line );
		varName = line;

		line = $0;
		gsub( ".*=[[:space:]]*", "", line );
		gsub( "[[:space:]]*;.*", "", line );
		factors[ varName ] = line;
		next;
	}

	/^[[:space:]]*float[[:space:]]+patt_/ {
		gsub( "^[[:space:]]*float[[:space:]]+patt_", "" );
		line = $0;

		# print "Processing pattern";

		gsub( "[[:space:]]*\\[", "", line );
		gsub( "[[:space:]]*\\].*", "", line );
		patternVar = varName;
		# print "Found pattern of var: " varName "; processing";
		pattern[ patternVar ] = "";

		line = $0;
		gsub( ".*{", "", line );
		gsub( "[[:space:]]", "" );

		flagEnd = gsub( "}.*", "", line );
		pattern[ patternVar ] = pattern[ patternVar ] line;
		if ( flagEnd != 0 ) {
			patternVar = "";
			patt++;
		}
		next;
	}

	/}[[:space:]]*;/ {
		if ( patternVar == "" ) {
			print "Error -- End of pattern out of scope";
			exit;
		}

		gsub( "}.*", "" );
		gsub( "[[:space:]]", "" );
		pattern[ patternVar ] = pattern[ patternVar ] $0;
		patternVar = "";
		patt++;
		next;
	}

	/,/ {
		gsub( "[[:space:]]", "" );
		pattern[ patternVar ] = pattern[ patternVar ] $0;
		next;
	}

	END { 
		# print "End processing file"; 
		if ( shp == 0 ) {
			print "No stencil-shape description found";
			exit;
		}
		if ( patt == 0 ) {
			print "No stencil-pattern description found";
			exit;
		}
		if ( shp < patt ) {
			print "More pattern than shape declarations";
			exit;
		}

		for( varName in shapes ) {
			# ANALYZE SHAPE
			# print varName ":" shapes[ varName ], " with " shapesDims[ varName ] " dimensions.";

			expectedData = 1;
			for( dim=0; dim<shapesDims[ varName ]; dim++ ) {
				# print "\t" dim ": from " rangeBegins[ varName, dim ] " to " rangeEnds[ varName, dim ];
				dimSize = 1 + rangeEnds[ varName, dim] - rangeBegins[ varName, dim ];
				expectedData = expectedData * dimSize;
			}

			# PATTERN
			if ( pattern[ varName ] == "" ) {
				print "Error -- No pattern declared for variable: " varName;
				exit;
			}
			numData = split( pattern[ varName ], data, "," );
			if ( numData != expectedData ) {
				print "Error -- The number of weights is not correct for the shape. Shape size: " expectedData ", num.weights: " numData;
				exit;
			}
#			for( i=1; i<=numData; i++ ) {
#				if ( data[i] == "" ) {
#					print "Error -- Empty weight detected in position " i-1 " of the pattern";
#					exit;
#				}
#				print "\t" data[i];
#			}

			# GENERATE KERNEL
			for( dim=0; dim<shapesDims[ varName ]; dim++ ) {
				indexes[ dim ] = rangeBegins[ varName, dim ];
			}
			dimNames[0] = "x";
			dimNames[1] = "y";
			dimNames[2] = "z";

			# Kernel header
			printf( "\n/*\n * Generic stencil skeleton\n * Generated kernel: %s\n */\n", varName );
			printf( "CTRL_KERNEL ( %s, GENERIC , DEFAULT , KHitTile_float mat , KHitTile_float copy, {\n", varName );
    			printf( "\tint %s = thr_i ;\n", dimNames[0] );
			if ( shapesDims[ varName ] > 1 ) 
    				printf( "\tint %s = thr_j ;\n", dimNames[1] );
			if ( shapesDims[ varName ] > 2 ) 
    				printf( "\tint %s = thr_k ;\n", dimNames[2] );

			# Update access expresion
			printf( "\n\thit( mat, %s", dimNames[0] );
			if ( shapesDims[ varName ] > 1 ) printf( ", %s", dimNames[1]);
			if ( shapesDims[ varName ] > 2 ) printf( ", %s", dimNames[2] );
			printf( " ) = (\n" );

			# Read access expresions
			firstData = 0;
			for( i=1; i<=numData; i++ ) {
				# Skip weights zero
				if ( data[i] != 0 ) {
					# Skip addition on the first non-zero weight	
					if ( firstData != 0 ) printf( "\t+\n");
					firstData = 1;

					# Write access expression
					printf("\t\t");
					if ( data[i] != 1 && data[i] != -1 ) printf("%s * ", data[i] );
					if ( data[i] == -1 ) printf("- ");
					printf("hit( copy");
					for( dim=0; dim<shapesDims[ varName ]; dim++ ) {
						printf(", %s", dimNames[dim] );
						if ( indexes[ dim ] < 0 ) {
							printf("-%d", -indexes[ dim ] );
						}
						else if ( indexes[ dim ] > 0 ) {
							printf("+%d", indexes[ dim ] );
						}

					}
					printf(" )");
				}

				# Advance indexes
				for( dim=shapesDims[ varName ]-1; dim>=0; dim-- ) {
					indexes[ dim ] ++;
					if ( indexes[ dim ] <= rangeEnds[ varName, dim ] )
						break;
					indexes[ dim ] = rangeBegins[ varName, dim ];
				}
			}

			# Close access expresions
			if ( factors[ varName ] == "" ) {
				print "Error -- No division factor declared for kernel: " varName;
				exit;
			}
			printf( "\t) / %s ;\n", factors[ varName ] );
			printf( "\n});\n" );
		}
	}
	' $1
