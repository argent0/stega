my_TEMP_DIR=`mktemp --tmpdir="." -d XXXXXXXX`
my_TEMP_DIR_FP=`readlink -f "$my_TEMP_DIR"`
TMP_INPUT_FILE=`mktemp --tmpdir="."`

QUIET=0
SILENCE="/dev/stdout"

my_ERROR () {
	echo "$@" > /dev/stderr
}

my_PRINT () {
	if [ $QUIET -eq 0 ]
	then
		echo "$@"
	fi
}

my_USAGE () {
	my_ERROR $1 "hides data into an image"
	my_ERROR
	my_ERROR USAGE 
	my_ERROR "$1 <data to hide> <image to use> <output image>"
}

my_FinalClean () {
	my_PRINT "Terminating""$1""."
	rm -r "$my_TEMP_DIR_FP"
	rm "$TMP_INPUT_FILE"
	exit
}

OPTIONS=`getopt --options q -n "$0" -- "$@"`
if [ $? != 0 ]; then my_FinalClean " abnormaly"; fi

eval set -- "$OPTIONS"

while true; do
	case "$1" in
		-q)	QUIET=1; SILENCE="/dev/null"; shift ;;
		--)	shift; break;;
		*)		break;;
	esac
done

if [[ $# -lt 3 ]]
then
	my_USAGE $0
	my_FinalClean " abnormaly"
fi

if [[ $1 == "-" ]]
then
	while read -r line
	do
		echo "$line" > "$TMP_INPUT_FILE"
	done
	INPUT_DATA=`readlink -f "$TMP_INPUT_FILE"`	#returns full path
else
	INPUT_DATA=`readlink -f "$1"`	#returns full path
fi

HIDEOUT_IMAGE=`readlink -f "$2"`
OUTPUT_IMAGE=`readlink -f "$3"`

cd "$my_TEMP_DIR_FP"

my_PRINT Decompressing png...
../png_extractor "$HIDEOUT_IMAGE" > $SILENCE || my_FinalClean " abnormaly"
my_PRINT Hiding the information...
../hide "$INPUT_DATA" raw rigged > $SILENCE || my_FinalClean " abnormaly"
my_PRINT Re-compressing
../png_restorer "$HIDEOUT_IMAGE" rigged "$OUTPUT_IMAGE" > $SILENCE || my_FinalClean " abnormaly"

cd ..

my_FinalClean " normally"
