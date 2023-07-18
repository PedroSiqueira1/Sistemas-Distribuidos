g++ process.cpp -o process -lstdc++

mkdir teste3

#TESTE 3

./coordinator

for i in 1 2 4 8 16 32 64 128;
do
    for j in $(seq 1 $i);
    do
        ./process 3 0 &
    done
    wait $(jobs -p)
    python3 cleanlog.py
    python3 validator.py ./log.txt 3 $i

    cp log.txt teste3/log_$i.txt
    cp resultado.txt teste3/resultado_$i.txt

    rm log.txt
    rm resultado.txt

done