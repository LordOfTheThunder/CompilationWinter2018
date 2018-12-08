numtests=24
i="1"
while [ $i -le $numtests ]
	do
		./../hw3 < t$i.in 2>&1 > t$i.res
   echo "run test #"$i"!"
		diff t$i.res t$i.out &> /dev/null
		if [[ $? != 0 ]] 
			then
				echo "Failed test #"$i"!"
				exit
		fi
		i=$[$i+1]
done

rm *.res

