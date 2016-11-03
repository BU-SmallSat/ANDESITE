cd "$(dirname ${BASH_SOURCE[0]})"
     	docker build -t gvoysey/andesite .


#!/bin/sh
# Use > 1 to consume two arguments per pass in the loop (e.g. each
# argument has a corresponding value to go with it).
# Use > 0 to consume one or more arguments per pass in the loop (e.g.
# some arguments don't have a corresponding value to go with it such
# as in the --default example).
# note: if this is set to > 0 the /etc/hosts part is not recognized ( may be a bug )
# while [[ $# > 1 ]]
# do
# key="$1"

# case $key in
#     -d|--data)
#     DATA="$2"
#     shift # past argument
#     ;;
#     -o|--output)
#     OUTPUT="$2"
#     shift # past argument
#     ;;   
#     *)
#             # unknown option
#     ;;
# esac
# shift # past argument or value
# done
# # echo "data is ${DATA} output is ${OUTPUT}"
# # if they asked for help or didn't give an argument
# if [[(! -n "${DATA}"  || ! -n "${OUTPUT}") ]]; 
# then
#     printf "ANDESITE Build Script.\n\n"
#     printf "Usage:\n"
#     printf "\t build.sh -h | --help\n"
#     printf "\t build.sh -d | --data <data_path> -o | --output <output_path>\n\n"
#     printf "Options:\n"
#     printf "\t -h --help \t\t Print this message and exit.\n" 
#     printf '\t -d --data \t\t Specifies the directory of the *.soundspeed, *.wind, *.bathymetry and *.sediment files.\n'
#     printf '\t -o --output \t\t Specify the directory to output results.\n\n'
# else
# 	#if [[ ! -d "${DATA}" || ! -d "${OUTPUT}" ]]; 
# 	#then
#    # 	printf "Incorrect directory specified for data or output."
#    # else
#     	cd "$(dirname ${BASH_SOURCE[0]})"
#     	docker build -t budja/wagner-mono .
#     #	docker run -it -v "$DATA":/opt/data:ro -v "$OUTPUT":/opt/output budja/wagner-mono /bin/bash
#     #fi    
# fi