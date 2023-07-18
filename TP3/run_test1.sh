g++ process.cpp -o process -lstdc++

mkdir teste1

#TESTE 1

for i in 1 2 4 8 16 32;
do
    for j in $(seq 1 $i);
    do
        ./process 10 2 &
    done
    wait $(jobs -p)
    python3 cleanlog.py
    python3 validator.py ./log.txt 10 $i


    cp log.txt teste1/log_$i.txt
    cp resultado.txt teste1/resultado_$i.txt

    rm log.txt
    rm resultado.txt
done
