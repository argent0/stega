my_TEMP_DIR=`mktemp --tmpdir="." -d XXXXXXXX`
my_TEMP_DIR_FP=`readlink -f "$my_TEMP_DIR"`
TMP_OUTPUT_FILE=`mktemp --tmpdir="$my_TEMP_DIR_FP"`
TMP_OUTPUT_FILE_FP=`readlink -f  "$TMP_OUTPUT_FILE"`

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

my_FinalClean () {
	my_PRINT "Terminating""$1""."
	rm -r "$my_TEMP_DIR_FP"
	exit
}

my_USAGE () {
	my_ERROR $1 
	my_ERROR "Recovers data from an image"
	my_ERROR
	my_ERROR USAGE 
	my_ERROR "$1 <image where data is hiden> <size of the hidden data>  <output file>"
	my_ERROR "<output file> defaults to stdout"
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

if [[ $# -lt 2 ]]
then
	my_USAGE $0
	my_FinalClean " abnormaly"
fi

if [[ $# == 3 ]]
then
	OUTPUT_FILE="$3"
else
	my_PRINT Using Stdout
	OUTPUT_FILE="/dev/stdout"
fi

HIDEOUT_IMAGE=`readlink -f "$1"`
DATA_SIZE="$2"

cd "$my_TEMP_DIR_FP"

my_PRINT Decompressing png...
../png_extractor "$HIDEOUT_IMAGE" > $SILENCE || my_FinalClean " abnormaly" 
my_PRINT "Recovering The Data..."
../recover raw "$DATA_SIZE" "$TMP_OUTPUT_FILE" > $SILENCE || my_FinalClean " abnormaly" 

cd ..

cat "$TMP_OUTPUT_FILE" > "$OUTPUT_FILE"

my_FinalClean " normally"
