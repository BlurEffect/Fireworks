/*
Particle system managing a single particle that simulates a rocket.
*/

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "FireworkParticleSystem.h"


class Projectile : public FireworkParticleSystem
{
public:
	Projectile() : FireworkParticleSystem(), launchAngle_(D3DXToRadian(0))
	{
	}

	Projectile(float launchAngle) : FireworkParticleSystem(), launchAngle_(D3DXToRadian(launchAngle))
	{
	}

	~Projectile(void)
	{
	}
	
	// returns a pointer to the single particle's position
	D3DXVECTOR3* getProjectilePosition()
	{
		return &particles_[0].position_;
	}

	// returns a pointer to the move direction of the particle (velocity with environmental influences)
	D3DXVECTOR3* getProjectileMoveDirection()
	{
		return &particleMoveDirection_;
	}

	bool isExploded(void)
	{
		return particles_[0].lifetime_ == 0;
	}

	virtual void update(void)
	{
		// Start particles, if necessary...
		startParticles();

		// Update the particles that are still alive...
		for (std::vector<Particle>::iterator p(particles_.begin()); p != particles_.end(); ++p)
		{
			if (p -> lifetime_ > 0)	// Update only if this particle is alive.
			{
				// Calculate the new position of the particle...

				// Vertical distance.
				float s = (p -> velocity_.y * p -> time_) + (EARTH_GRAVITY * p -> time_ * p -> time_);
				
				particleMoveDirection_.x = (p -> velocity_.x * p -> time_);
				particleMoveDirection_.y = (p -> velocity_.y * p -> time_) + (EARTH_GRAVITY * p -> time_ * p -> time_);
				particleMoveDirection_.z = (p -> velocity_.z * p -> time_);

				p -> position_.x = particleMoveDirection_.x + origin_.x;
				p -> position_.y = particleMoveDirection_.y + origin_.y;
				p -> position_.z = particleMoveDirection_.z + origin_.z;

				p -> time_ += timeIncrement_;
				--(p -> lifetime_);

				if (p -> lifetime_ == 0)	// Has this particle come to the end of it's life?
				{
					--particlesAlive_;		// If so, terminate it.
				}
			}
		}

		// Create a pointer to the first vertex in the buffer
		// Also lock it, so nothing else can touch it while the values are being inserted.
		POINTVERTEX *points;
		points_ -> Lock(0, 0, (void**)&points, 0);

		// Fill the vertex buffers with data...
		int P(0);

		// Now update the vertex buffer - after the update has been
		// performed, just in case this particle has died in the process.

		for (std::vector<Particle>::iterator p(particles_.begin()); p != particles_.end(); ++p)
		{
			if (p -> lifetime_ > 0)
			{
				points[P].position_.y = p -> position_.y;
				points[P].position_.x = p -> position_.x;
				points[P].position_.z = p -> position_.z;

				points[P].color_ = p -> colour_;
				++P;
			}
		}

		points_ -> Unlock();
	}

	// the projectile will be launched by this angle
	float launchAngle_;

private:
	D3DXVECTOR3 particleMoveDirection_;

	virtual void startSingleParticle(std::vector<Particle>::iterator& p)
	{
		if (p == particles_.end()) return;	// Safety net - if there are no dead particles, don't start any new ones...

		// Reset the particle's time (for calculating it's position with s = ut+0.5t*t)
		p -> time_ = 0;

		float angle = D3DXToRadian(launchAngle_ + 90.0f);

		// calculate the particle's horizontal and depth components.
		p -> velocity_.x = launchVelocity_ * (float)cos(angle);
		p -> velocity_.y = launchVelocity_ * (float)sin(angle);
		p -> velocity_.z = 0;	// projectiles only fly in the x and y directions

		// set the colour for the particle
		getRandomColour(&(p->colour_));
		// set lifetime 
		p -> lifetime_ = getRandomLifetime();
		// set particle size
		p -> size_ = getRandomSize();

		++particlesAlive_;
	}
};

#endif