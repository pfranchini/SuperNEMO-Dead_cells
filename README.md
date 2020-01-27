# SuperNEMO-Dead_cells
SuperNEMO scripts to study the dead cells in the tracker:

- kills cells from an ideal generator root output (dead_cells)
- translate the root file into a brio file using https://github.com/SuperNEMO-DBD/SN-IEgenerator/tree/master/FLTranslate
- reconstruct the brio file with Falaise (flreconstruct)
- runs the Sensitivity module on the reconstruted brio file
- calculate the tracks reconstruction efficiency on the sensitivity module output (efficiency)                                         
- plots efficiency vs number of dead cells

## Build:
```
Make
```

## Code:

Kills cells from an ideal generator root output:
```
./dead_cells -i <ideal_input.root> -o <output.root> -n <number_of_dead_cells> (-d <list_of_dead_cells>)
```

Calculate the efficiency for some reconstructed output:
```
./efficiency -i <reconstructed_output.root>
```

Run the full process for one <number_of_dead_cells>:
```
process_one.sh <number_of_dead_cells> (-d <list_of_dead_cells>)
```

Run the full process for several [0..2000] dead cells:
```
process.sh
```
