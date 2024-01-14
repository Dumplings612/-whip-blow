#include <SFML/Graphics.hpp>
#include <iostream>

class Whip
{
	// Конструктор класса (если не добавляются новые данные, данные ниже будут использоваться по умолчанию)
public:
	Whip( int point_count = 24, bool gravity = true, double stiffness = 6,  double v_ampl = 4, double length = 0.5, int time_steps = 60000, bool x_border = false):
		m_M(1), m_length(length),m_c(stiffness),m_point_count(point_count), m_time_steps(time_steps), m_gravity(gravity), m_v_ampl(v_ampl), m_x_border(x_border),m_W(1000),m_H(1000),
		m_r(7),m_window(sf::RenderWindow(sf::VideoMode(1000,1000), "Whip")), g(9.81)
	{
		//ограничение по скорости подсчёта
		m_window.setFramerateLimit(800);
		//подготовка массива масс и создание текстуры на которой будут рисоваться точки
		prepare();
		//шрифт, размер, цвет текста
		prepare_text();
	}

	~Whip()=default;

	 void calculations()
	{
//Проверка открыто ли окно
		 while (m_window.isOpen())
		 {
			 //
			 std::vector<double> x = std::vector<double>(m_point_count);

			 for (auto i = 0; i < m_point_count; ++i)
			 {
				 x[i] = 1.0 / (m_point_count - 1) * i;
			 }

			 std::vector<double> y = std::vector<double>(m_point_count);

			 std::vector<double> velocity_x = std::vector<double>(m_point_count, 0);
			 std::vector<double> velocity_y = std::vector<double>(m_point_count, 0);

			 std::vector<double> F_x = std::vector<double>(m_point_count - 1, 0);
			 std::vector<double> F_y = std::vector<double>(m_point_count - 1, 0);

			 std::vector<double> T = std::vector<double>(m_time_steps, 0);

			 for (auto i = 0; i < m_time_steps; ++i)
			 {
				 T[i] = 50.0 / (m_time_steps - 1) * i;
			 }

			 auto dt = T[1] - T[0];

			 std::cout << "dt/tau: " << dt / tau << '\n';
			 x[0] = 0;
			 y[0] = 0;
			 velocity_x[0] = 0;
			 velocity_y[0] = 0;

			 auto a = 1.0 / m_point_count;

			 std::vector<double>gravity_force = std::vector<double>(m_point_count, 0);

			 if (m_gravity)
			 {
				 for (auto i = 0; i < m_point_count; ++i)
				 {
					 gravity_force[i] = (m_mass_array[i] * g) / (m_c * m_length);
				 }
			 }
			 else
			 {
				 gravity_force = std::vector<double>(m_point_count, 0);
			 }

			 auto flag = 1;
			 auto frame_count = 500;
			 auto i = 0;

			 for (auto t = 0; t < 10000; ++t)
			 {
//закрытие окошка
				 while (m_window.pollEvent(m_event))
				 {
					 switch (m_event.type)
					 {
					 case sf::Event::Closed:
						 m_window.close();
						 break;
					 default:
						 break;
					 }
				 }

				 i += 1;
				 auto delta_x = x[1] - x[0];
				 auto delta_y = y[1] - y[0];

				 auto l1 = std::sqrt(delta_x * delta_x + delta_y * delta_y);

				 F_x[0] = (l1 - a) * delta_x / l1;
				 F_y[0] = (l1 - a) * delta_y / l1;

				 if (i == frame_count)
				 {
					 velocity_y[0] = flag * m_v_ampl;

					 flag *= -1;

					 frame_count += 500;
				 }

				 for (auto j = 1; j < m_point_count - 1; ++j)
				 {
					 delta_x = x[j + 1] - x[j];
					 delta_y = y[j + 1] - y[j];

					 l1 = std::sqrt(delta_x * delta_x + delta_y * delta_y);

					 F_x[j] = (l1 - a) * delta_x / l1;
					 F_y[j] = (l1 - a) * delta_y / l1;

					 velocity_x[j] = velocity_x[j] + (F_x[j] - F_x[j - 1]) * dt * (4.0 * m_pi * m_pi);
					 velocity_y[j] = velocity_y[j] + (F_y[j] - F_y[j - 1] - gravity_force[j]) * dt * (4.0 * m_pi * m_pi);

				 }

				 velocity_x[m_point_count - 1] = velocity_x[m_point_count - 1] + (
					 -F_x[m_point_count - 1 - 1]) * dt * (4.0 * m_pi * m_pi);
				 velocity_y[m_point_count - 1] = velocity_y[m_point_count - 1] + (
					 -F_y[m_point_count - 1 - 1] - gravity_force[m_point_count - 1]) * dt * (4.0 * m_pi * m_pi);

				 for (auto j = 0; j < m_point_count; ++j)
				 {
					 x[j] = x[j] + velocity_x[j] * dt;
					 y[j] = y[j] + velocity_y[j] * dt;
					 if (m_x_border)
					 {
						 if (1 <= j <= m_point_count - 1)
						 {
							 x[j] = std::max(x[j], x[j - 1]);
						 }

					 }

				 }
				 //очищаем текстуру
				 background_texture.clear();
				 //создаем кружки и сетку
				 sf::CircleShape circle(m_r);

				 circle.setFillColor(sf::Color(48, 186, 143));

				 draw_lines();

				 for (auto j = 0; j < m_point_count; ++j)
				 {
					 circle.setPosition(50 + x[j] * (m_W - 100) - m_r, m_H / 2.0 + y[j] / 10.0 * m_H / 2.0 - m_r);
					 //50-отступ слева(справа), m_W/m_H-ширина/высота окошка, m_r - радиус кружочка (т.к рисуется не от центра)
					 //делим на 10, чтобы не вылетало за грни экрана
					 background_texture.draw(circle);
				 }

				 //рисуем спрайты и передаем на него текстурку
				 m_background.setTexture(background_texture.getTexture());

				 m_window.clear();

				 m_window.draw(m_background);
				 //добавили нолик
				 draw_text();
				 //отображаем на экране
				 m_window.display();

			 }
		}
		

	}

private:

	void prepare()
	{
		m_mass_array = std::vector<double>();
		auto sum = 0.0;
		m_M_sum = 0.0;

		for (auto i = 0; i < m_point_count; ++i)
		{
			m_mass_array.push_back((4.0 - (4.0 - 1.0) / m_point_count * i) / 300);
			sum += m_mass_array[i];
		}

		for (auto i = 0; i < m_point_count; ++i)
		{
			m_mass_array[i] /= sum;
			m_M_sum += m_mass_array[i];
		}

		std::cout << "Summarnaya massa: "<<m_M_sum << '\n';

		tau = std::sqrt(m_M_sum / m_c) * m_pi;
		std::cout << "Harakternoe vremya: " << tau << '\n';

		background_texture.create(1000, 1000);
		background_texture.clear();

	}

	void prepare_text()
	{
		m_font.loadFromFile("consolas.ttf");
		m_text.setFont(m_font);
		m_text.setCharacterSize(24U);
		m_text.setFillColor(sf::Color::White);
	}
	//рисуем линии для сетки (класс, соединяющий 2 последовательные точки)
	void draw_lines()
	{
		sf::VertexArray axes(sf::Lines, 4);
		axes[0].position = sf::Vector2f(50, m_H - 50);
		axes[1].position = sf::Vector2f(50, 50);
		axes[2].position = sf::Vector2f(50, m_H/2);
		axes[3].position = sf::Vector2f(m_W-50, m_H / 2);
		background_texture.draw(axes);

		sf::VertexArray horizontals(sf::Lines, 42);
		for (auto i = 0; i < 21; ++i)
		{
			horizontals[2 * i].position = sf::Vector2f(50, 50 + (m_H - 100) / 20 * i);
			horizontals[2 * i].color = sf::Color(255, 255, 255, 100);

			horizontals[2 * i + 1].position = sf::Vector2f(m_W - 50, 50 + (m_H - 100) / 20 * i);
			horizontals[2 * i + 1].color = sf::Color(255, 255, 255, 100);
		}

		background_texture.draw(horizontals);

		sf::VertexArray verticals(sf::Lines, 40);
		for (auto i = 0; i < 20; ++i)
		{
			verticals[2 * i].position = sf::Vector2f(50 + (m_W - 100) / 19 * i, 50);
			verticals[2 * i].color = sf::Color(255, 255, 255, 100);

			verticals[2 * i + 1].position = sf::Vector2f(50 + (m_W - 100) / 19 * i, m_H-50 );
			verticals[2 * i + 1].color = sf::Color(255, 255, 255, 100);
		}

		background_texture.draw(verticals);
	}
	//рисуем нолик
	void draw_text()
	{
		m_text.setPosition(25, m_H/2-14);
		m_text.setString("0");
		m_window.draw(m_text);
	}

	int m_M;
	double m_length;
	double m_c;
	int m_point_count;
	int m_time_steps;
	bool m_gravity;
	double m_v_ampl;
	bool m_x_border;
	double m_M_sum;
	std::vector<double> m_mass_array;
	double m_pi = 3.14159265358979323846;

	sf::RenderWindow m_window;
	sf::Event m_event;

	sf::RenderTexture background_texture;
	sf::Sprite m_background;

	sf::Text m_text;
	sf::Font m_font;

	int m_H;
	int m_W;
	int m_r;

	double g;
	double tau;
	
};


int main()
{

	auto whip = Whip(40,true,25,2);

	whip.calculations();

	return 0;
}
