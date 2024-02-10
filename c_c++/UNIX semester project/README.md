Jak coś wycrashuje to do usunięcia można użyć
ipcrm --all (usuwa wszysko IPC, alternatywnie ipcs --all i usunąć tylko semafor)
i usunąć z /dev/shm odpowiedni plik.


Samo utworzenie pamięci i semaforów
./build/daemon --create

Samo usunięcie pamięci i semaforów
./build/daemon --delete

Proste użycie (musi być wcześniej stworzona)
./build/daemon --run

Można też
./build/daemon --create --delete --run


Jako demo prosty zapis
./build/daemon --print
./build/daemon --add first
./build/daemon --add second
./build/daemon --print
./build/daemon --remove second
./build/daemon --print
itd

-v włącza wypisywanie co jest w mapie
