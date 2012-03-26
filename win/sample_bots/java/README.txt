Pentru compilare este necesara instalarea unui compilator de java (JDK) si adaugarea caii catre javac in PATH.
Compilarea tuturor botilor se realizeaza cu "compile.cmd".
Exemplu de rulare:
python "playgame.py" --verbose --map_file "maps\maze\maze_p02_01.map" "java -cp sample_bots/java RandomBot" "java -cp sample_bots/java RandomBot"
(classpath nu trebuie sa aiba trailing slash).