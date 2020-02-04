#
# Usage: ./process_one.sh <number_of_dead_cells> <list_of_dead_cells>
#
# Runs the full process once:
#
# - kills cells from an ideal generator root output
# - translate the root file into a brio file
# - reconstruct the brio file
# - runs the sensitivity module on the reconstruted brio file
# - calculate the tracks reconstruction efficiency on the sensitivity module output

#==========================================================================================================

DEAD_CELLS=$1
DEAD_FILE=$2
FALAISE="/vols/build/snemo/Falaise.build/bin/"                   # Falaise binaries location
INPUT="~/SuperNEMO/SN-IEgenerator/output/illumination_1k.tsim"  # Idealized generator output
WHERE=data                                                       # output directory
FILE=output                                                      # name of the output file

#==========================================================================================================

# Purge data
rm -f ${WHERE}/${FILE}.root
rm -f ${WHERE}/${FILE}.brio
rm -f ${WHERE}/${FILE}-reco.brio
rm -f ${WHERE}/${FILE}-reco.root
rm -f ${WHERE}/${FILE}-reco.log
rm -f ${WHERE}/${FILE}-reco-sensitivity.conf

# Kill cells
if [ -z "${DEAD_FILE}" ]; then
    ./dead_cells -i ${INPUT} -o ${WHERE}/${FILE}.root -n ${DEAD_CELLS}
else
    ./dead_cells -i ${INPUT} -o ${WHERE}/${FILE}.root -n ${DEAD_CELLS} -d ${DEAD_FILE}
fi

if [[ "$?" -ne "0" ]]; then
    exit
fi

# Convert root into brio
/home/hep/pfranchi/SuperNEMO/SN-IEgenerator.ceschia/FLTranslate.build/fltranslate -i ${WHERE}/${FILE}.root -o ${WHERE}/${FILE}.brio 

# Reconstruct simulation with dead channels
${FALAISE}flreconstruct -i ${WHERE}/${FILE}.brio -p data/official-2.0.0.conf -o ${WHERE}/${FILE}-reco.brio &> /dev/null

if [[ "$?" -ne "0" ]]; then
    exit
fi

# Runs the sensitivity module
cd data
/home/hep/pfranchi/SuperNEMO/MC/do_sensitivity.sh ${FILE}-reco
cd ..

# Find the efficiency of track reconstruction and writes into a file
./efficiency -i ${WHERE}/${FILE}-reco.root 




