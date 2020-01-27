#
# Usage: ./process_test.sh <number_of_dead_cells> <list_of_dead_cells> 
#
# Runs the full process:
#
# - kills cells from an ideal generator root output
# - translate the root file into a brio file
# - reconstruct the brio file
# - runs the sensitivity module on the reconstruted brio file
# - calculate the tracks reconstruction efficiency on the sensitivity module output

#==========================================================================================================

N=100
DEAD_CELLS=$1
DEAD_FILE=$2
FALAISE="/vols/build/snemo/Falaise.build/bin/"                   # Falaise binaries location
INPUT="~/SuperNEMO/SN-IEgenerator/output/illumination_1k.tsim"  # Idealized generator output
WHERE=data                                                       # output directory
FILE=output                                                      # name of the output file

#==========================================================================================================

echo -e "Run "$N" simulation for "${DEAD_CELLS}"dead cells each...\n"

rm -f efficiency_for_DC${DEAD_CELLS}.txt

for i in {0..100}
do

    echo "Run: "$i
    echo -e "=========\n"

# Kill cells                                                                                                                                                                
    if [ -z "${DEAD_FILE}" ]; then
	./dead_cells -i ${INPUT} -o ${WHERE}/${FILE}.root -n ${DEAD_CELLS}
    else
	./dead_cells -i ${INPUT} -o ${WHERE}/${FILE}.root -n ${DEAD_CELLS} -d ${DEAD_FILE}
    fi
        
    # Convert root into brio
    /home/hep/pfranchi/SuperNEMO/SN-IEgenerator.ceschia/FLTranslate.build/fltranslate -i ${WHERE}/${FILE}.root -o ${WHERE}/${FILE}.brio
    
    # Reconstruct simulation with dead channels
    ${FALAISE}flreconstruct -i ${WHERE}/${FILE}.brio -p data/official-2.0.0.conf -o ${WHERE}/${FILE}-reco.brio &> /dev/null
    
    # Runs the sensitivity module
    cd data
    /home/hep/pfranchi/SuperNEMO/MC/do_sensitivity.sh ${FILE}-reco
    cd ..
    
    # Find the efficiency of track reconstruction and writes into a file
    ./efficiency -i ${WHERE}/${FILE}-reco.root  > eff.log

    EFFICIENCY=`cat eff.log | grep Efficiency | awk '{print $2}'`
    ZERO=`cat eff.log | grep Zero | awk '{print $3}'`
    MORE=`cat eff.log | grep More | awk '{print $5}'`
    rm -f eff.log

    echo $DEAD_CELLS $EFFICIENCY $ZERO $MORE >> efficiency_for_DC${DEAD_CELLS}.txt

done


