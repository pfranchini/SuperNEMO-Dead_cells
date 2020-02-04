# Usage: ./process.sh [no arguments]
#
# Runs the full process:
#
# - kills cells from an ideal generator root output
# - translate the root file into a brio file
# - reconstruct the brio file
# - runs the sensitivity module on the reconstruted brio file
# - calculate the tracks reconstruction efficiency on the sensitivity module output
# - plots

#==========================================================================================================

FALAISE="/vols/build/snemo/Falaise.build/bin/"                   # Falaise binaries location
INPUT="~/SuperNEMO/SN-IEgenerator/output/illumination_1k.tsim"  # Idealized generator output
WHERE=data                                                       # output directory
FILE=output                                                      # name of the output file

#==========================================================================================================

#echo "DEAD_CELLS EFFICIENCY ZERO MORE" > efficiency.txt
rm -f efficiency.txt

for DEAD_CELLS in {0..2000..100}
do 

    echo ${DEAD_CELLS}
    
    # Kill cells
    ./dead_cells -i ${INPUT} -o ${WHERE}/${FILE}.root -n ${DEAD_CELLS} 
    if [[ "$?" -ne "0" ]]; then
	echo "Problem with dead_cells... exit!"
	exit
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
    ./efficiency -i ${WHERE}/${FILE}-reco.root > eff.log
    
    EFFICIENCY=`cat eff.log | grep Efficiency | awk '{print $2}'`
    ZERO=`cat eff.log | grep Zero | awk '{print $3}'`
    MORE=`cat eff.log | grep More | awk '{print $5}'`
    SHORT=`cat eff.log | grep Short | awk '{print $3}'`
    rm -f eff.log
    
    echo $DEAD_CELLS $EFFICIENCY $ZERO $MORE $SHORT >> efficiency.txt
    
done

python plot_efficiency.py
