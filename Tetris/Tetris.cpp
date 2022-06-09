#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <sstream> 

using namespace std;
using namespace sf;

const int M = 20; /*!< Deklaracja stalej zmiennej oznaczajacej wysokosc pola gry */
const int N = 10; /*!< Deklaracja stalej zmiennej oznaczajacej szerokosc pola gry */
int field[M][N] = { 0 }; /*!< Deklaracja tablicy zawierajacej wysokosc i szerokosc pola gry */
int points = 0; /*!< Deklaracja zmiennej oznaczajacej ilosc zdobytych punktow przez gracza */
string disScore; /*!< Deklaracja zmiennej pomocniczej, dzieki ktorej mozemy zamienic punkty liczone w typie int na typ string i wypisac wynik na ekranie jako tekst */



int figures[7][4] =
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};/*!< Deklaracja tablicy figur gry Tetris*/

/*!
* @brief Struktura opisujaca maksymalna ilosc pojedynczych kostek klocka w grze, tworzaca dwie tablice na podstawie struktury: a[4] i b[4]
*
* @param listnode::x x - pierwsze koordynaty
* @param listnode::y y - drugie koordynaty
*/
struct Point
{
	int x, y;
} a[4], b[4];


/*!
* @brief Funkcja sprawdzajaca, czy nie wykraczamy w danym momencie poza granice pola gry
* @return 1
*/
bool check()
{
	for (int i = 0; i < 4; i++)
		if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return 0;
		else if (field[a[i].y][a[i].x]) return 0;

	return 1;
}
/*!
* @brief Funkcja sprawdzajaca, czy gra sie zakonczyla przegraną
* @return 1 jesli tak
* @return 0 jesli nie
*/
bool checkIfGameEnded()
{
	for (int i = 0; i < N; i++)
	{
		if (field[1][i])
		{
			return 1;
		}
	}
	return 0;
}

int main()

{

	srand(time(0)); /*!< Deklaracja funkcji losujacej srand */
	RenderWindow window(VideoMode(800, 1000), "Tetris"); /*!< Deklaracja funkcji generujacej okno o szerokosci 800 pixeli na 1000 pixeli wysokosci */
	sf::Music menuMusic; /*!< Inicjalizacja muzyki w menu gry, jesli nie uda sie odtworzyc sciezki dzwieku, w konsoli pojawi sie komunikat "ERROR" */
	if (!menuMusic.openFromFile("D:\\Tetris\\Build\\menu.ogg"))
	{
		std::cout << "ERROR" << endl;
	}
	/*!
	* @brief Funkcja rozpoczynajaca odtwarzanie muzyki w menu gry
	*/
	menuMusic.play();
	Event menuEnter; /*!< Deklaracja procedury pozwalajacej na rozpoczecie gry klikajac klawisz ENTER oraz wyswietlajaca grafike menu */
	do {

		window.pollEvent(menuEnter);
		menuEnter.type == Event::KeyPressed;
		Texture textureMENU;
		textureMENU.loadFromFile("D:\\Tetris\\Build\\MENU.png");
		Sprite spriteMENU;
		spriteMENU.setTexture(textureMENU);
		spriteMENU.setPosition(0, 0);
		window.draw(spriteMENU);
		window.display();


	} while (menuEnter.key.code != Keyboard::Enter);
	/*!
	* @brief Funkcja zatrzymujaca muzyke z menu po wcisnieciu klawisza ENTER
	*/
	menuMusic.stop();

	//inicjalizacja tla
	Texture textureBackground; /*!< Inicjalizacja tekstury tla */
	textureBackground.loadFromFile("D:\\Tetris\\Build\\tlo.png"); /*!< zaladowannie tekstury tla */
	Sprite spriteBackground;
	spriteBackground.setTexture(textureBackground); 
	spriteBackground.setPosition(0, 0); /*!< Ustawienie tekstury na srodek */
	window.clear(); /*!< Oczyszczanie okna */
	window.draw(spriteBackground); /* Przygotowanie tekstury tla do wyswietlenia */
	window.display(); /*!< Wyswietlenie tekstury tla */
	//inicjalizacja muzyki
	sf::Music music; /*!< Inicjalizacja muzyki podczas gry, jesli nastapi blad zaladowania pliku, program wyswietli w konsoli komunikat "ERROR" */
	if (!music.openFromFile("D:\\Tetris\\Build\\soundtrack.ogg"))
	{
		std::cout << "ERROR" << endl;
	}
	/*!
	* @brief Funkcja rozpoczynajaca odtwarzanie muzyki podczas gry
	*/
	music.play();
	/*!
	* @brief Funkcja zapetlajaca odtwarzanie muzyki podczas gry
	*/
	music.setLoop(true);
	Texture texture; /*!< Deklaracja tekstury dla klocka w grze */
	texture.loadFromFile("D:\\Tetris\\Build\\13.png"); /*!< Zaladowanie tekstur dla klockow w grze */
	Sprite sprite(texture);
	sprite.setTextureRect(IntRect(0, 0, 288, 36));  /*!< Wycinanie pojedynczych kwadratow z tekstury wszystkich klockow */

	// Zmienne dla ruchu poziomego i obrotu
	int dx = 0; /*!< Zmienna okreslajaca ruch poziomy klocka */
	bool rotate = 0;  /*!< Zmienna okreslajaca obrot klocka */
	int colorNum = 1; /*!< Zmienna okreslajaca kolor klocka */
	bool beginGame = true; /*!< Zmienna okreslajaca stan rozpoczecia gry */
	int n = rand() % 7; /*!< Zmienna losujaca jeden z kolorow klocka */

	// Zmienne dla timera i opóźnienia
	float timer = 0; /*!< Zmienna dla licznika czasu */
	float delay = 0.3; /*!< Zmienna powodujaca opoznienie spadania klocka w grze */

	// Zegar (timer)
	Clock clock;
	// Główna pętla aplikacji: wykonywana, kiedy okno jest otwarte
	while (window.isOpen())
	{
		// Otrzymujemy czas, co przeszło z początku odliczania, i konwertujemy jego na sekundy
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;
		// Obsługa zdarzeń w pętli
		Event event;
		while (window.pollEvent(event))
		{
			// zamykamy okno 
			if (event.type == Event::Closed)
				window.close();
			// Sprawdzamy , czy na klawiaturze został naciśnięty przycisk
			if (event.type == Event::KeyPressed)
				// Эта кнопка – стрелка вверх?…
				if (event.key.code == Keyboard::Up) rotate = true;
			//Sprawdzamy , czy na klawiaturze została naciśnięta strzałka w lewo
				else if (event.key.code == Keyboard::Left) dx = -1;
			// Sprawdzamy , czy na klawiaturze została naciśnięta strzałka w prawo
				else if (event.key.code == Keyboard::Right) dx = 1;

			if (event.type == sf::Event::Resized)
			{
				sf::FloatRect view(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(view));
			}
		}
		// Sprawdzamy, czy została naciśnięta strzałka w dół, jeśli tak, to tetramina jest przyspieszana
		if (Keyboard::isKeyPressed(Keyboard::Down))
			delay = 0.05;

		// Ruch poziomy
		for (int i = 0; i < 4; i++)
		{
			b[i] = a[i];
			a[i].x += dx;
		}

		// Jeśli tetramina wyszła z pola po przeprowadzce? To zwracamy stare koordynaty 
		if (!check())
			for (int i = 0; i < 4; i++)
				a[i] = b[i];

		// Obrót
		if (rotate)
		{
			Point p = a[1]; /*!< Ustawienie srodka dla obrotu klocka */
			for (int i = 0; i < 4; i++)
			{
				int x = a[i].y - p.y; //y-y0
				int y = a[i].x - p.x; //x-x0
				a[i].x = p.x - x;
				a[i].y = p.y + y;
			}
			// Tetramina wyszła poza pole po zawracaniu? To zwracamy stare koordynaty 
			if (!check()) {
				for (int i = 0; i < 4; i++)
					a[i] = b[i];
			}

		}

		// Przesuwanie Tetrimino w dół (Timer tick)
		if (timer > delay)
		{
			for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].y += 1; }
			if (!check())
			{
				for (int i = 0; i < 4; i++) field[b[i].y][b[i].x] = colorNum;
				colorNum = 1 + rand() % 7;
				n = rand() % 7;
				for (int i = 0; i < 4; i++)
				{
					a[i].x = figures[n][i] % 2;
					a[i].y = figures[n][i] / 2;
				}
				/*!
				* @brief Funkcja sprawdzajaca, czy nastapil koniec gry, jesli tak, to zatrzyjmuje soundtrack z gry, odtwarza dzwiek konca gry oraz wyswietla ekran koncowy razem z wynikiem i czeka na wcisniecie klawisza enter
				*/
				if (checkIfGameEnded()) 
				{

					music.stop();
					sf::Music koniecMusic;
					if (!koniecMusic.openFromFile("D:\\Tetris\\Build\\koniec.ogg"))
					{
						std::cout << "ERROR" << endl;
					}
					koniecMusic.play();
					Event koniecEnter;
					do {

						window.pollEvent(koniecEnter);
						koniecEnter.type == Event::KeyPressed;
						Texture textureKONIEC;
						textureKONIEC.loadFromFile("D:\\Tetris\\Build\\KONIEC.jpg");
						Sprite spriteKONIEC;
						spriteKONIEC.setTexture(textureKONIEC);
						spriteKONIEC.setPosition(0, 0);
						window.draw(spriteKONIEC);
						//stworzenie czcionki
						sf::Font font; 
						// zaladowanie czcionki z dysku
						if (!font.loadFromFile("D:\\Tetris\\Build\\Pixeboy-z8XGD.ttf"))
						{
							cout << "Error loading font\n";
						}

						sf::Text score;
						score.setFont(font);
						score.setString(disScore);
						score.setCharacterSize(80);
						score.setStyle(sf::Text::Bold);
						score.setFillColor(Color::Magenta);
						//score.setColor(sf::Color::Black);
						score.setPosition(350, 500);
						window.draw(score);
						//score.setString(disScore);
						//dotad
						window.display();


					} while (koniecEnter.key.code != Keyboard::Enter);

					return 0;
				}
			}
			timer = 0;

		}

		// SPRAWDŹ LINIĘ
		int k = M - 1;   /*!< Zmienna pozwalajaca na sprawdzanie lini */
		for (int i = M - 1; i > 0; i--)
		{
			int count = 0;  /*!< Zmienna pozwalajaca na zliczanie punktow */
			for (int j = 0; j < N; j++)
			{
				if (field[i][j])
				{
					count++;
				}
				field[k][j] = field[i][j];
			}
			if (count < N) k--;
			else if (count == N)
			{
				//Liczenie punktów (linia usunięta)
				//N * ilość punktów (np. 1, 10)
				points = points + N * 10;
				cout << points << endl;
				stringstream ss;  /*!< deklaracja stringstream, by moc przekonwertowac typ int na string, aby moc wyswietlic wynik z customowa czcionka */
				ss << points;

				ss >> disScore;


			}
		}


		//// Pierwsze pojawienie się klocka w polu gry
		if (beginGame)
		{
			beginGame = false;
			n = rand() % 7;
			for (int i = 0; i < 4; i++)
			{
				a[i].x = figures[n][i] % 2;
				a[i].y = figures[n][i] / 2;
			}
		}


		dx = 0; rotate = 0; delay = 0.3;
		window.clear(); 
		window.draw(spriteBackground);
		for (int i = 0; i < M; i++)
			for (int j = 0; j < N; j++)
			{
				if (field[i][j] == 0) continue;
				/*!
				* @brief Funckcja wycinajaca klocki
				*/
				sprite.setTextureRect(IntRect(field[i][j] * 0, 0, 36, 36));
				/*!
				* @brief Funkcja ustawiajaca klocka
				*/
				sprite.setPosition(j * 36, i * 36);
				/*!
				* @brief Funkcja przesuwajaca klocki
				*/
				sprite.move(28, 31);

			// Wykonanie sprita
				window.draw(sprite);
			}
		for (int i = 0; i < 4; i++)
		{
			/*!
			* @brief Funckja ustawiajaca wylosowany kolor dla klocka
			*/
			sprite.setTextureRect(IntRect(colorNum * 36, 0, 36, 36));

			/*!
			* @brief Funkcja ustawiajaca cale klocki
			*/
			sprite.setPosition(a[i].x * 36, a[i].y * 36);

			sprite.move(28, 31); // przesuwanie

			// Wykonanie sprita
			window.draw(sprite);

		}

		// Tworzenia okna
		/*!
		* @brief Wyswietlenie ekranu gry
		*/
		window.display(); 

	}
	return 0;
}