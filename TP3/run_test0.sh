g++ process.cpp -o process -lstdc++

mkdir teste0

#TESTE 0

for i in $(seq 1 2);
do
    ./process 10 2 &
done
wait $(jobs -p)

python3 cleanlog.py
python3 validator.py ./log.txt 10 2 

cp log.txt teste0/
cp resultado.txt teste0/
 
rm log.txt
rm resultado.txt

