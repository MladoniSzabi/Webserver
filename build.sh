for file in src/*.cpp; do
    g++ -g -c $file -I./include --std=c++20 -static -o ./out/${file#*/}.o
done

for file in src/utility/*.cpp; do
    g++ -g -c $file -I./include --std=c++20 -static -o ./out/${file#*/}.o
done

for file in src/linux/*.cpp; do
    g++ -g -c $file -I./include --std=c++20 -static -o ./out/${file#*/}.o
done

ar crf libwebserver.a out/*.o out/utility/* out/linux/*