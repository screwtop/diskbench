for drive in 7k250 7200.8 wd1200jb ST38420A ST320414A-3.05-3EC0B11G ST320414A-3.05-3EC0B55W ST33232A-3.05-VHE29764 IBM-DCHS09F-CLAR09-HP53 ST38420A-3.07-7AZ0ESBD; do 
  echo "${drive}: "

  echo "  * access time histogram"
  pl -maxrows 20000 -png -o r-dist-${drive}.png -scale 1 -prefab dist fld=4 data=${drive}-random.dat binsize=1 barwidth=0.05 yrange="0 1200" xrange="0 50" color=blue;

  echo "  * access time scatter plot"
  pl -maxrows 15000 -png -o r-scatter-${drive}.png data=${drive}-random.dat ploticus/access-times_read_scatter.ploticus

  echo "  * access time heatmap"
  pl -maxrows 20000 -png -o r-heatmap-${drive}.png -scale 1 -prefab heatmap x=3 y=4 data=${drive}-random.dat xbinsize=0.002 xrange="0 1" ybinsize=1 yrange="0 50" colorlist="gray(0.0),gray(0.2),gray(0.4),gray(0.6),gray(0.8),gray(0.9)" zerocolor=white

  echo "  * sequential read transfer rate"
  pl -maxrows 15000 -png -o s-${drive}.png data=${drive}-seq.dat ploticus/sequential_read.ploticus

done

