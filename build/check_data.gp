set terminal pngcairo size 1200,600
set output 'data_check.png'
set multiplot layout 1,2
set title 'Первые 100 точек траектории'
set grid
plot 'trajectory_full.csv' every ::1::100 using 2:3 with points title 'Планета' linecolor rgb 'blue', \
     'trajectory_full.csv' every ::1::100 using 4:5 with points title 'Спутник' linecolor rgb 'red'
set title 'Относительная траектория (первые 100 точек)'
plot 'trajectory_relative.csv' every ::1::100 using 1:2 with points title 'Спутник' linecolor rgb 'green'
unset multiplot
