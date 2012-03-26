Pentru compilare este necesara instalarea unui compilator de java (JDK) si adaugarea caii catre javac in PATH.
Compilarea botului se realizeaza cu "compile.cmd".
Exemplu de rulare:
python "playgame.py" --verbose --map_file "maps\maze\maze_p02_01.map" "java -cp dev MyBot" "java -cp dev MyBot"
(classpath nu trebuie sa aiba trailing slash).

Exemple de rulare:
1) MyBot.class se afla in directorul "dev" de pe acelasi nivel cu playgame.py
python "playgame.py" --verbose --map_file "maps\maze\maze_p02_01.map" "java -cp dev MyBot" "java -cp dev MyBot"
(classpath nu trebuie sa aiba trailing slash, deci "dev" in loc de "dev/").
2) MyBot.class impreuna cu toate .class-urile dependente se afla pe acelasi nivel cu playgame.py:
python "playgame.py" --verbose --map_file "maps\maze\maze_p02_01.map" "java MyBot" "java MyBot"
