/* Copyright © 2026 BestProject Team */
#include "nightmode.h"

#include <base/math.h>

#include <engine/shared/config.h>

#include <game/client/components/camera.h>
#include <game/client/gameclient.h>
#include <game/client/render.h>

#include <algorithm>

void CBcNightMode::OnInit()
{
	m_LightTexture = Graphics()->LoadTexture("mapres/light.png", IStorage::TYPE_ALL);
}

void CBcNightMode::OnRender()
{
	if(!g_Config.m_BcNightMode && !g_Config.m_BcGlow && !g_Config.m_BcVignette)
		return;

	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		return;

	float ScreenX0, ScreenY0, ScreenX1, ScreenY1;
	Graphics()->GetScreen(&ScreenX0, &ScreenY0, &ScreenX1, &ScreenY1);

	const vec2 Center = GameClient()->m_Camera.m_Center;
	Graphics()->MapScreenToInterface(Center.x, Center.y, GameClient()->m_Camera.m_Zoom);

	float Width = 0.0f;
	float Height = 0.0f;
	Graphics()->CalcScreenParams(Graphics()->ScreenAspect(), GameClient()->m_Camera.m_Zoom, &Width, &Height);
	const float Left = Center.x - Width * 0.5f;
	const float Top = Center.y - Height * 0.5f;

	vec2 LocalPos = Center;
	if(GameClient()->m_Snap.m_pLocalCharacter != nullptr)
		LocalPos = GameClient()->m_LocalCharacterPos;

	if(g_Config.m_BcNightMode)
	{
		const float Darkness = std::clamp(g_Config.m_BcNightModeDarkness / 100.0f, 0.0f, 1.0f);
		Graphics()->TextureClear();
		Graphics()->DrawRect(Left, Top, Width, Height, ColorRGBA(0.02f, 0.02f, 0.06f, Darkness), IGraphics::CORNER_ALL, 0.0f);

		const float Radius = (float)g_Config.m_BcNightModeLightRadius;
		const ColorRGBA LightColor = color_cast<ColorRGBA>(ColorHSLA(g_Config.m_BcNightModeLightColor, true));
		Graphics()->TextureSet(m_LightTexture);
		Graphics()->BlendAdditive();
		Graphics()->QuadsBegin();
		Graphics()->SetColor(LightColor);
		IGraphics::CQuadItem Quad(LocalPos.x - Radius, LocalPos.y - Radius, Radius * 2.0f, Radius * 2.0f);
		Graphics()->QuadsDrawTL(&Quad, 1);
		Graphics()->QuadsEnd();
		Graphics()->BlendNormal();
	}

	if(g_Config.m_BcGlow)
	{
		const float GlowStrength = std::clamp(g_Config.m_BcGlowStrength / 100.0f, 0.0f, 1.0f);
		const float GlowRadius = 64.0f + 96.0f * GlowStrength;
		Graphics()->TextureSet(m_LightTexture);
		Graphics()->BlendAdditive();
		Graphics()->QuadsBegin();
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			if(!GameClient()->m_Snap.m_aCharacters[i].m_Active)
				continue;

			const vec2 Pos = GameClient()->m_aClients[i].m_RenderPos;
			ColorRGBA Color = ColorRGBA(1.0f, 1.0f, 1.0f, 0.35f * GlowStrength);
			if(GameClient()->IsTeamPlay())
			{
				if(GameClient()->m_aClients[i].m_Team == TEAM_RED)
					Color = ColorRGBA(1.0f, 0.4f, 0.4f, 0.35f * GlowStrength);
				else if(GameClient()->m_aClients[i].m_Team == TEAM_BLUE)
					Color = ColorRGBA(0.4f, 0.6f, 1.0f, 0.35f * GlowStrength);
			}
			else
			{
				const int Team = GameClient()->m_Teams.Team(i);
				if(Team)
					Color = GameClient()->GetDDTeamColor(Team, 0.6f).WithAlpha(0.35f * GlowStrength);
			}

			Graphics()->SetColor(Color);
			IGraphics::CQuadItem Quad(Pos.x - GlowRadius, Pos.y - GlowRadius, GlowRadius * 2.0f, GlowRadius * 2.0f);
			Graphics()->QuadsDrawTL(&Quad, 1);
		}
		Graphics()->QuadsEnd();
		Graphics()->BlendNormal();
	}

	if(g_Config.m_BcVignette)
	{
		const float VignetteSize = minimum(Width, Height) * 0.15f;
		const ColorRGBA VignetteColor(0.0f, 0.0f, 0.0f, 0.35f);
		Graphics()->TextureClear();
		Graphics()->DrawRect(Left, Top, Width, VignetteSize, VignetteColor, IGraphics::CORNER_ALL, 0.0f);
		Graphics()->DrawRect(Left, Top + Height - VignetteSize, Width, VignetteSize, VignetteColor, IGraphics::CORNER_ALL, 0.0f);
		Graphics()->DrawRect(Left, Top, VignetteSize, Height, VignetteColor, IGraphics::CORNER_ALL, 0.0f);
		Graphics()->DrawRect(Left + Width - VignetteSize, Top, VignetteSize, Height, VignetteColor, IGraphics::CORNER_ALL, 0.0f);
	}

	Graphics()->MapScreen(ScreenX0, ScreenY0, ScreenX1, ScreenY1);
}