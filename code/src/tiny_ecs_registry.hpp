#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	// TODO: A1 add a LightUp component
	ComponentContainer<SpriteSheet> spriteSheets;
	ComponentContainer<KeyframeAnimation> keyframeAnimations;
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<InfectTimer> infectTimers;
	ComponentContainer<LostLife> lostLifeTimer;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<PlayerEffects> playerEffects;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<Human> humans;
	ComponentContainer<Zombie> zombies;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<Platform> platforms;
	ComponentContainer<Background> backgrounds;
	ComponentContainer<Wall> walls;
	ComponentContainer<Spike> spikes;
	ComponentContainer<Wheel> wheels;
	ComponentContainer<Climbable> climbables;
	ComponentContainer<Book> books;
	ComponentContainer<TextBox> textboxes;
	ComponentContainer<Collectible> collectible;
	ComponentContainer<Overlay> overlay;
	ComponentContainer<Dangerous> dangerous;
	ComponentContainer<Fading> fading;
	ComponentContainer<Door> doors;
	ComponentContainer<Bounce> bounce;
	ComponentContainer<Boss> bosses;
	ComponentContainer<ZombieDeathTimer> zombieDeathTimers;
	ComponentContainer<CutsceneTimer> cutSceneTimers;


	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		// TODO: A1 add a LightUp component
		registry_list.push_back(&spriteSheets);
		registry_list.push_back(&keyframeAnimations);
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&infectTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&playerEffects);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&humans);
		registry_list.push_back(&zombies);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
		registry_list.push_back(&platforms);
		registry_list.push_back(&backgrounds);
		registry_list.push_back(&walls);
		registry_list.push_back(&spikes);
		registry_list.push_back(&wheels);
		registry_list.push_back(&climbables);
		registry_list.push_back(&books);
		registry_list.push_back(&collectible);
		registry_list.push_back(&overlay);
		registry_list.push_back(&lostLifeTimer);
		registry_list.push_back(&dangerous);
		registry_list.push_back(&fading);
		registry_list.push_back(&doors);
		registry_list.push_back(&bounce);
		registry_list.push_back(&bosses);
		registry_list.push_back(&zombieDeathTimers);
		registry_list.push_back(&cutSceneTimers);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Camera info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Camera info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;