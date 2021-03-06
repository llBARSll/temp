﻿
#include "SFML\Graphics.hpp"
#include "SFML\Window.hpp"
#include "SFML\System.hpp"

#include <math.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>


using namespace sf;

class Bullet
{
    public:
        Sprite shape;
        Texture* texture;

        Bullet(Texture* texture, Vector2f position)
        {
            this->shape.setTexture(*texture);

            this->shape.setScale(1.f, 1.f);
            this->shape.setRotation(45.f);

            this->shape.setPosition(position);

        }

        ~Bullet() {};

};


class Player
{
    public:
        Sprite shape;
        Texture *texture;
        int HP;
        int HPmax;

        std::vector<Bullet> bullets;

        Player(Texture *texture)
        {
            this->HPmax = 10;
            this->HP = this->HPmax;

            this->texture = texture;
            this->shape.setTexture(*texture);

            this->shape.setScale(2, 2);
        }

        ~Player(){}

};

class Enemy
{
public:
    Sprite shape;
    Texture* texture;
    int HP;
    int HPmax;

    Enemy(Texture* texture, Vector2u windowSize)
    {
        this->HPmax = rand()% 3 + 1;
        this->HP = this->HPmax;

        this->shape.setTexture(*texture);

        this->shape.setScale(3, 3);

        this->shape.setPosition(windowSize.x - this->shape.getGlobalBounds().width, rand()%(int)(windowSize.y - this->shape.getGlobalBounds().height));
    }

    ~Enemy() {}

};




int main()
{
    srand(time(NULL));

    RenderWindow window(VideoMode(800, 600), "Starcity 0.0", Style::Default);
    window.setFramerateLimit(60);


    //Init text
    Font font;
    font.loadFromFile("Fonts/Roboto-Light.ttf");


    //Init textures
    Texture playerTex;
    playerTex.loadFromFile("Textures/Player.png");

    Texture enemyTex;
    enemyTex.loadFromFile("Textures/Enemy.png");

    Texture bulletTex;
    bulletTex.loadFromFile("Textures/Bullet.png");



    //Player init
    int score = 0;
    Player player(&playerTex);
    int shootTimer = 20;
    
    Text hpText;
    hpText.setFont(font);
    hpText.setCharacterSize(12.f);
    hpText.setFillColor(Color::White);

    //Enemy init
    int enemySpawnTimer = 0;
    std::vector<Enemy>enemies;

    Text eHpText;
    eHpText.setFont(font);
    eHpText.setCharacterSize(12.f);
    eHpText.setFillColor(Color::White);
    
    //UI init
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10.f, 10.f);

    Text gameOver;
    gameOver.setFont(font);
    gameOver.setCharacterSize(40);
    gameOver.setFillColor(Color::Red);
    gameOver.setPosition(player.shape.getPosition().x, player.shape.getPosition().y);
    gameOver.setString("GAME OVER");


    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        if(player.HP > 0)
      {
        //Player
        if (Keyboard::isKeyPressed(Keyboard::W))
            player.shape.move(0.f, -10.f);
        if (Keyboard::isKeyPressed(Keyboard::A))
            player.shape.move(-10.f, 0.f);
        if (Keyboard::isKeyPressed(Keyboard::S))
            player.shape.move(0.f, 10.f);
        if (Keyboard::isKeyPressed(Keyboard::D))
            player.shape.move(10.f, 0.f);

        hpText.setPosition(player.shape.getPosition().x, player.shape.getPosition().y - hpText.getGlobalBounds().height);
        hpText.setString(std::to_string(player.HP) + "/" + std::to_string(player.HPmax));

        //Collision with window
        if (player.shape.getPosition().x <= 0) //Left
            player.shape.setPosition(0.f, player.shape.getPosition().y);
        if (player.shape.getPosition().x >= window.getSize().x - player.shape.getGlobalBounds().width) //Right
            player.shape.setPosition(window.getSize().x - player.shape.getGlobalBounds().width, player.shape.getPosition().y);
        if (player.shape.getPosition().y <= 0) //Top
            player.shape.setPosition(player.shape.getPosition().x, 0.f);
        if (player.shape.getPosition().y >= window.getSize().y - player.shape.getGlobalBounds().height) //Bottom
            player.shape.setPosition(player.shape.getPosition().x, window.getSize().y - player.shape.getGlobalBounds().height);


        //Update controls
        if (shootTimer < 10)
            shootTimer++;

        if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= 10) //Shooting
        {
            player.bullets.push_back(Bullet(&bulletTex, player.shape.getPosition()));
            shootTimer = 0; // reset timer
        }

        //Bullets
        for (size_t i = 0; i < player.bullets.size(); i++)
        {
            //move
            player.bullets[i].shape.move(20.f, 0.f);

            //Out of window bounds
            if (player.bullets[i].shape.getPosition().x > window.getSize().x)
            {
                player.bullets.erase(player.bullets.begin() + i);
                break;
            }

            //enemy collision
            for (size_t k = 0; k < enemies.size(); k++)
            {
                if (player.bullets[i].shape.getGlobalBounds().intersects(enemies[k].shape.getGlobalBounds()))
                {
                    if (enemies[k].HP <= 1)
                    {
                        score += enemies[k].HPmax;
                        enemies.erase(enemies.begin() + k);
                    }
                    else
                        enemies[k].HP--; // ENEMY TAKE DAMAGE

                    player.bullets.erase(player.bullets.begin() + i);
                    break;
                }
            }

        }

        //enemy spawn
        if (enemySpawnTimer < 25)
            enemySpawnTimer++;

        if (enemySpawnTimer >= 25)
        {
            enemies.push_back(Enemy(&enemyTex, window.getSize()));
            enemySpawnTimer = 0; //reset timer
        }

        for (size_t i = 0; i < enemies.size(); i++)
        {
            enemies[i].shape.move(-6.f, 0.f);

            if (enemies[i].shape.getPosition().x <= 0 - enemies[i].shape.getGlobalBounds().width)
            {
                enemies.erase(enemies.begin() + i);
                break;
            }
            if (enemies[i].shape.getGlobalBounds().intersects(player.shape.getGlobalBounds()))
            {
                enemies.erase(enemies.begin() + i);

                player.HP--; //PLAYER TAKE COLLISION DAMAGE
                break;
            }
        }

        //UI update
        scoreText.setString("Score: " + std::to_string(score));

    }

        //Draw ===========================================================
        window.clear();

        //player
        window.draw(player.shape);

        //Bullets
        for (size_t i = 0; i < player.bullets.size(); i++)
        {
            window.draw(player.bullets[i].shape);
        }

        //enemy
        for (size_t i = 0; i < enemies.size(); i++)
        {
            eHpText.setString(std::to_string(enemies[i].HP) + "/" + std::to_string(enemies[i].HPmax));
            eHpText.setPosition(enemies[i].shape.getPosition().x, enemies[i].shape.getPosition().y - eHpText.getGlobalBounds().height);
            window.draw(eHpText);
            window.draw(enemies[i].shape);
        }

        //UI
        window.draw(scoreText);
        window.draw(hpText);

        if (player.HP <= 0)
            window.draw(gameOver);


        window.display();
    }


    return 0;
}
