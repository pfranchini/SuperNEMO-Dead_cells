#
# Runs the full process:
#
# - kills cells from an ideal generator root output
# - translate the root file into a brio file
# - reconstruct the brio file
# - runs the sensitivity module on the reconstruted brio file
# - calculate the tracks reconstruction efficiency on the sensitivity module output

#==========================================================================================================

FALAISE="/vols/build/snemo/Falaise.build/bin/"                   # Falaise binaries location
INPUT="~/SuperNEMO/SN-IEgenerator/output/illumination_10k.tsim"  # Idealized generator output
WHERE=ceschia/toyillumination_10k                                                       # output directory
FILE=simulation                                                      # name of the output file

#==========================================================================================================

#echo "DEAD_CELLS EFFICIENCY ZERO MORE" > efficiency.txt
rm -f efficiency.txt

for DEAD_CELLS in {0..2000..100}
do 

    echo ${DEAD_CELLS}
    
    # Kill cells
    ./dead_cells -i ${INPUT} -o ${WHERE}/${FILE}-${DEAD_CELLS}.root -n ${DEAD_CELLS} 
    if [[ "$?" -ne "0" ]]; then
	exit
    fi

    # Convert root into brio
    /home/hep/pfranchi/SuperNEMO/SN-IEgenerator.ceschia/FLTranslate.build/fltranslate -i ${WHERE}/${FILE}-${DEAD_CELLS}.root -o ${WHERE}/${FILE}-${DEAD_CELLS}.brio

    # Reconstruct simulation with dead channels
    ${FALAISE}flreconstruct -i ${WHERE}/${FILE}-${DEAD_CELLS}.brio -p data/official-2.0.0.conf -o ${WHERE}/${FILE}-${DEAD_CELLS}-reco.brio &> /dev/null

    # Runs the sensitivity module
    #cd data
    #/home/hep/pfranchi/SuperNEMO/MC/do_sensitivity.sh ${FILE}-${DEAD_CELLS}-reco
    #cd ..

    # Find the efficiency of track reconstruction and writes into a file
    #./efficiency -i ${WHERE}/${FILE}-${DEAD_CELLS}-reco.root > eff.log
    
    #EFFICIENCY=`cat eff.log | grep Efficiency | awk '{print $2}'`
    #ZERO=`cat eff.log | grep Zero | awk '{print $3}'`
    #MORE=`cat eff.log | grep More | awk '{print $5}'`
    #rm -f eff.log
    
    #echo $DEAD_CELLS $EFFICIENCY $ZERO $MORE >> efficiency.txt
    

done

#python plot_efficiency.py
