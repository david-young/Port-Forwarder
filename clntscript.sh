#modify d to change the number of loops. Leave c alone.
c=0
d=20000
echo "Script starting"
while [ $c -lt $d ]
do
	./clnt 192.168.0.11 1000 1000 8007 &
	(( c++ ))
done
echo "Script done running after $c loops"

