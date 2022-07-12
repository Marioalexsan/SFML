#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/SpriteBatch.hpp>

#include <doctest/doctest.h>

#include <SystemUtil.hpp>

bool operator==(const sf::Image& first, const sf::Image& second)
{
    if (first.getSize() != second.getSize())
        return false;

    for (unsigned int y = 0; y < first.getSize().y; y++)
    {
        for (unsigned int x = 0; x < first.getSize().x; x++)
        {
            if (first.getPixel({x, y}) != second.getPixel({x, y}))
                return false;
        }
    }

    return true;
}

TEST_CASE("sf::SpriteBatch class - [graphics]")
{
    sf::RenderWindow window;
    window.create(sf::VideoMode({1024, 768}), "", sf::Style::None);

    REQUIRE(window.setActive());

    sf::Image blue, green, red;

    blue.create({100, 100}, sf::Color(0, 0, 255));
    green.create({100, 100}, sf::Color(0, 255, 0));
    red.create({100, 100}, sf::Color(255, 0, 0));

    sf::Texture blueTex, greenTex, redTex;
    REQUIRE(blueTex.loadFromImage(blue));
    REQUIRE(greenTex.loadFromImage(green));
    REQUIRE(redTex.loadFromImage(red));

    sf::RenderTexture target;
    REQUIRE(target.create({1000, 1000}));

    sf::CircleShape triangle(80.f, 3);
    sf::CircleShape circle(80.f);
    sf::CircleShape pentagon(80.f, 5);

    triangle.setOutlineThickness(5);
    circle.setOutlineThickness(5);
    pentagon.setOutlineThickness(5);

    triangle.setOutlineColor(sf::Color::Black);
    circle.setOutlineColor(sf::Color::Black);
    pentagon.setOutlineColor(sf::Color::Black);

    SUBCASE("BatchMode::Deferred")
    {
        // Ensure that Deferred renders drawables similarly
        // to RenderTarget in all cases

        triangle.setPosition({100.f, 100.f});
        circle.setPosition({150.f, 100.f});
        pentagon.setPosition({200.f, 100.f});

        triangle.setTexture(&blueTex);
        circle.setTexture(&greenTex);
        pentagon.setTexture(&redTex);

        sf::SpriteBatch batch;
        batch.setBatchMode(sf::SpriteBatch::BatchMode::Deferred);

        target.clear();
        target.draw(triangle);
        target.draw(circle);
        target.draw(pentagon);
        target.display();

        sf::Image sampleResult = target.getTexture().copyToImage();

        target.clear();
        batch.batch(triangle);
        batch.batch(circle);
        batch.batch(pentagon);
        target.draw(batch);
        target.display();

        sf::Image batchResult = target.getTexture().copyToImage();

        CHECK(sampleResult == batchResult);
    }

    SUBCASE("BatchMode::TextureSort - no overlap")
    {
        // Ensure that TextureSort renders drawables similarly
        // to RenderTarget when drawables do not overlap

        triangle.setPosition({100.f, 100.f});
        circle.setPosition({300.f, 300.f});
        pentagon.setPosition({500.f, 500.f});

        triangle.setTexture(&blueTex);
        circle.setTexture(&greenTex);
        pentagon.setTexture(&redTex);

        sf::SpriteBatch batch;
        batch.setBatchMode(sf::SpriteBatch::BatchMode::TextureSort);

        target.clear();
        target.draw(triangle);
        target.draw(circle);
        target.draw(pentagon);
        target.display();

        sf::Image sampleResult = target.getTexture().copyToImage();

        // Purposefully batched in a different order!
        target.clear();
        batch.batch(pentagon);
        batch.batch(triangle);
        batch.batch(circle);
        target.draw(batch);
        target.display();

        sf::Image batchResult = target.getTexture().copyToImage();

        CHECK(sampleResult == batchResult);
    }

    SUBCASE("BatchMode::TextureSort - overlap with same texture")
    {
        // Ensure that TextureSort renders drawables similarly
        // to RenderTarget when drawables using the same texture overlap
        // and are batched in the same order

        triangle.setPosition({100.f, 100.f});
        circle.setPosition({150.f, 100.f});
        pentagon.setPosition({200.f, 100.f});

        // Since the outline does not have a texture, use nullptr
        // to match it with the body
        triangle.setTexture(nullptr);
        circle.setTexture(nullptr);
        pentagon.setTexture(nullptr);

        sf::SpriteBatch batch;
        batch.setBatchMode(sf::SpriteBatch::BatchMode::TextureSort);

        target.clear();
        target.draw(triangle);
        target.draw(circle);
        target.draw(pentagon);
        target.display();

        sf::Image sampleResult = target.getTexture().copyToImage();

        // Batch using same order as drawing
        target.clear();
        batch.batch(triangle);
        batch.batch(circle);
        batch.batch(pentagon);
        target.draw(batch);
        target.display();

        sf::Image batchResult = target.getTexture().copyToImage();

        CHECK(sampleResult == batchResult);
    }

    SUBCASE("BatchMode::DepthSort - no overlap")
    {
        // This test is the same as TextureSort!

        triangle.setPosition({100.f, 100.f});
        circle.setPosition({300.f, 300.f});
        pentagon.setPosition({500.f, 500.f});

        triangle.setTexture(&blueTex);
        circle.setTexture(&greenTex);
        pentagon.setTexture(&redTex);

        sf::SpriteBatch batch;
        batch.setBatchMode(sf::SpriteBatch::BatchMode::DepthSort);

        target.clear();
        target.draw(triangle);
        target.draw(circle);
        target.draw(pentagon);
        target.display();

        sf::Image sampleResult = target.getTexture().copyToImage();

        // Purposefully batched in a different order!
        target.clear();
        batch.batch(pentagon);
        batch.batch(triangle);
        batch.batch(circle);
        target.draw(batch);
        target.display();

        sf::Image batchResult = target.getTexture().copyToImage();

        CHECK(sampleResult == batchResult);
    }

    SUBCASE("BatchMode::DepthSort - overlap with same texture")
    {
        // This test is the same as TextureSort!

        triangle.setPosition({100.f, 100.f});
        circle.setPosition({150.f, 100.f});
        pentagon.setPosition({200.f, 100.f});

        // Since the outline does not have a texture, use nullptr
        // to match it with the body
        triangle.setTexture(nullptr);
        circle.setTexture(nullptr);
        pentagon.setTexture(nullptr);

        sf::SpriteBatch batch;
        batch.setBatchMode(sf::SpriteBatch::BatchMode::DepthSort);

        target.clear();
        target.draw(triangle);
        target.draw(circle);
        target.draw(pentagon);
        target.display();

        sf::Image sampleResult = target.getTexture().copyToImage();

        // Batch using same order as drawing
        target.clear();
        batch.batch(triangle);
        batch.batch(circle);
        batch.batch(pentagon);
        target.draw(batch);
        target.display();

        sf::Image batchResult = target.getTexture().copyToImage();

        CHECK(sampleResult == batchResult);
    }

    SUBCASE("BatchMode::DepthSort - explicit draw order using depth")
    {
        // This test is the same as TextureSort!

        triangle.setPosition({100.f, 100.f});
        circle.setPosition({150.f, 100.f});
        pentagon.setPosition({200.f, 100.f});

        triangle.setTexture(&blueTex);
        circle.setTexture(&greenTex);
        pentagon.setTexture(&redTex);

        sf::SpriteBatch batch;
        batch.setBatchMode(sf::SpriteBatch::BatchMode::DepthSort);

        target.clear();
        target.draw(triangle);
        target.draw(circle);
        target.draw(pentagon);
        target.display();

        sf::Image sampleResult = target.getTexture().copyToImage();

        // Batch calls are in a different order
        // However, the drawables are rendered the same way
        // if depth is specified from furthest to closest
        target.clear();
        batch.batch(pentagon, 1.f); // Front
        batch.batch(triangle, 3.f); // Back
        batch.batch(circle, 2.f);   // Middle
        target.draw(batch);
        target.display();

        sf::Image batchResult = target.getTexture().copyToImage();

        CHECK(sampleResult == batchResult);
    }
}
