
#include <iostream>
#include "DataLoader.h"
#include "HashTable.h"
#include "AVLTree.h"
#include "GUI.h"

int main() {
    setlocale(LC_ALL, "Russian");

    int recordsCount;
    Students_entry* records = DataLoader::loadStudents(20, recordsCount, "students.txt");

    HashTable hashTable(5);
    for (int i = 0; i < recordsCount; i++) {
        hashTable.insert(records[i]);
    }
    hashTable.print();

    AVLTree concertTree;
    auto concerts = DataLoader::loadConcertsData("concerts.txt");
    for (const auto& concert : concerts) {
        concertTree.insert(concert);
    }
    concertTree.print(std::cout);

    /*
    GUI gui(hashTable, concertTree);
    gui.run();
    */

    delete[] records;
    return 0;
}



/*
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 200, 200 }), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
}
*/