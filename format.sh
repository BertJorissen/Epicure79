#!/bin/bash

sed -e 's/#pragma/\/\/#pragma/' \
	-e 's/#if/\/\/#if/' \
	-e 's/#el/\/\/#el/' \
	-e 's/#end/\/\/#end/' |
	clang-format -style=file |
	sed -e's/\/\/ *#pragma/#pragma/' \
		-e 's/\/\/ *#if/#if/' \
		-e 's/\/\/ *#el/#el/' \
		-e 's/\/\/ *#end/#end/'
