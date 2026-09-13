/* Copyright © 2026 BestProject Team */
#ifndef GAME_CLIENT_COMPONENTS_BESTCLIENT_NIGHTMODE_H
#define GAME_CLIENT_COMPONENTS_BESTCLIENT_NIGHTMODE_H

#include <engine/graphics.h>

#include <game/client/component.h>

class CBcNightMode : public CComponent
{
public:
	int Sizeof() const override { return sizeof(*this); }
	void OnInit() override;
	void OnRender() override;

private:
	IGraphics::CTextureHandle m_LightTexture;
};

#endif