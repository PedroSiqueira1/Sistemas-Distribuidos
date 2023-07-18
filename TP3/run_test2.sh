g++ process.cpp -o process -lstdc++

mkdir teste2

#TESTE 2

./coordinator

for i in 1 2 4 8 16 32 64;
do
    for j in $(seq 1 $i);
    do
        ./process 5 1 &
    done
    wait $(jobs -p)
    python3 cleanlog.py
    python3 validator.py ./log.txt 5 $i

    cp log.txt teste2/log_$i.txt
    cp resultado.txt teste2/resultado_$i.txt

    rm log.txt
    rm resultado.txt

done
