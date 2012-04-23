#include "../../Application.h"
#include "../../Game.h"
#include "../../Person.h"
#include "../../Sprite.h"
#include "Elevator.h"
#include "Queue.h"

using namespace OT::Item::Elevator;

/// The seconds at regular speed requried to stress a person from zero to max.
const static double kSecondsUntilStressed = 30;


Queue::Queue(Elevator * e)
:	GameObject(e->game),
	elevator(e)
{
	called = false;
	callTime = 0;
}

Queue::~Queue()
{
	for (People::iterator ip = people.begin(); ip != people.end();) {
		LOG(DEBUG, "forcing passenger %p ahead", (*ip));
		(*(ip++))->journey.next();
	}
}

void Queue::addPerson(Person * p)
{
	people.push_back(p);
	if (!called) {
		called = true;
		callTime = game->time.absolute;
		elevator->called(this);
	}
}

void Queue::removePerson(Person * p)
{
	people.remove(p);
}

void Queue::advance(double dt)
{
	double dta = game->time.dta;
	
	for (People::iterator ip = people.begin(); ip != people.end();) {
		Person * p = *(ip++);
		p->stress += 1.0 / kSecondsUntilStressed / Time::kBaseSpeed * dta;
		if (p->stress >= 1.0) p->journey.next();
	}
}

void Queue::Render(sf::RenderTarget & target) const
{
	bool steppingInside = false;
	
	//Start the queue 16 pixels away from the elevator.
	int x = 16;
	
	Sprite s;
	s.SetImage(app->bitmaps["simtower/elevator/people"]);
	s.SetCenter(direction == Elevator::kUp ? 16 : 0, 24);
	
	for (People::const_iterator ip = people.begin(); ip != people.end(); ip++) {
		Person * p = *ip;
		
		//If people are stepping into an elevator car, we have to draw the frontmost person
		//differently.
		bool stepping = (steppingInside && people.front() == p);
		
		//If the person is stepping inside we may move the x coordinate back by 16 pixels. This is
		//because the queue is formed starting 16 pixels away from the elevator. The stepping ani-
		//mations are designed to be drawn touching the elevator which would be 0 pixels. And since
		//we start at x = 16 we may move it back by 16 to start at 0.
		if (stepping) x -= 16;
		
		//Calculate the texture subrect for this person.
		int type = p->type;
		sf::IntRect sr;
		sr.Left   = type * 32;
		sr.Right  = sr.Left + 16;
		sr.Top    = (stepping ? 24 : 0);
		sr.Bottom = (stepping ? 48 : 24);
		if (direction == Elevator::kDown) {
			sr.Left  += 16;
			sr.Right += 16;
		}
		
		//Calculate the person's position.
		sf::Vector2f pos;
		pos.y = -(floor - elevator->position.y) * 36;
		pos.x = (direction == Elevator::kUp ? -x : elevator->size.x * 8 + x);
		
		//Decide what color to use based on stress level.
		sf::Color color = sf::Color::Black;
		if (p->stress > 0.8) color = sf::Color(255, 0, 0);
		else if (p->stress > 0.6) color = sf::Color(255, 128, 128);
		
		//Draw the person.
		s.SetColor(color);
		s.SetSubRect(sr);
		s.SetPosition(pos);
		target.Draw(s);
		
		//Move the queue and abort if we've reached our max drawing width.
		x += (stepping ? 16 : p->getWidth());
		if (x >= width) break;
	}
}
