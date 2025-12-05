# un pattern est unidirectionnel
# syntaxe 
# pattern (nom du pattern){
# source	Fonction source ou liste de fonction en cas de pattern triangulaire
# target	Fonction destination
# cycle	Nombre de cycle autorisant le chainage
#}
# le mot cle all specifie que la fonction peut etre chainee avec tous type de fonction

pattern(resize) {
	sources	all;
	targets	rndsat;
	cycle		4;
}

pattern(mac) {
	sources	mul;
	targets	add;
	cycle		2;
}
pattern(mac+) {
	sources	mul, mul;
	targets	add;
	cycle		2;
}
pattern(3add){
	sources	add, add;
	targets	add;
	cycle		2;
}

