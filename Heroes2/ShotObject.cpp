#include "ShotObject.h"
#include "GameManager.h"
#include "Unit.h"
#include "Animation.h"
#include <math.h>
#include "Grid.h"

ShotObject::ShotObject( Unit* owner, string name, SHOT_TYPE type, SDL_Point position )
: m_Owner( owner )
, m_ShotType( type )
{
    m_Name = name;
    m_Position = position;
}

ShotObject::ShotObject( string ownerName, ImagePtr currImg, SHOT_TYPE type, vector<SDL_Point> positions )
: m_ShotType( type )
, m_Positions( positions )
{
    if( m_Positions.size() )
        m_Position = m_Positions[0];
    m_CurImage = currImg;
    m_Name = ownerName;
}

void ShotObject::Initialize()
{
    auto& Game = GameManager::GetSingleton();
    if( !m_CurImage )
        m_CurImage = make_shared<Image>( Game.FindImage( m_Name ) );
    if( m_IsActive || !m_CurImage || !m_Owner )
        return;
    if( auto img = m_Owner->GetImage() )
    {
        auto start = img->GetRect();
        if( m_Owner->GetFlip() == SDL_FLIP_NONE )
        {
            start.x += m_Position.x;
            start.y += m_Position.y;
        }
        else
        {
            start.x += start.w - m_Position.x;
            start.y += m_Position.y;
        }
        start.w = m_CurImage->GetRect().w;
        start.h = m_CurImage->GetRect().h;
        m_CurImage->SetRect( start );
        m_CurImage->SetTexCenter( 0, 0 );
        if( Unit* target = m_Owner->GetAttackedUnit() )
        {
            target->SetPosition();
            if( auto targetImg = target->GetImage() )
            {
                auto end = targetImg->GetRect();
                m_Trajectory.x = end.x + end.w / 2.0f - start.x;
                m_Trajectory.y = end.y + end.h / 2.0f - start.y;
                float distance = m_Trajectory.Length();
                double angleRad = atan( m_Trajectory.y / m_Trajectory.x );
                m_Rotation = float( angleRad * 180.0f / M_PI );
                if( m_Owner->GetFlip() != SDL_FLIP_NONE )
                    m_Rotation -= 180.0f;
                auto anim = m_Owner->GetAnimation( m_Owner->GetState() );
                {
                    float time = anim.m_FrameTime;
                    if( m_ShotType == SHOT_TYPE::RAY )
                    {
                        m_Speed = distance;
                        m_LifeTime = time * 3;
                    }
                    else
                    {
                        m_Speed = eWindow::WIDTH * 3;
                        m_LifeTime = distance / m_Speed;
                    }
                    m_IsActive = true;
                    m_Trajectory = m_Trajectory.Normalized();
                }
            }
        }
    }
}

void ShotObject::Actualize()
{
    auto& Game = GameManager::GetSingleton();
    if( !m_IsActive || m_Owner != Game.GetActiveUnit().get() )
        return;
    float dt = Game.GetDeltaTime();
    m_CurrentTime += dt;
    if( m_LifeTime < 0.0f )
        Reset();
    m_LifeTime -= dt;
    if( m_ShotType == SHOT_TYPE::BULLET )
    {
        auto rect = m_CurImage->GetRect();
        
        rect.y += int( m_Trajectory.y * m_Speed * m_CurrentTime );
        rect.x += int( m_Trajectory.x * m_Speed * m_CurrentTime );
        if( rect.y > eWindow::HEIGHT )
            rect.y = eWindow::HEIGHT;
        else if( rect.y < 0 )
            rect.y = 0;
        if( rect.x > eWindow::WIDTH )
            rect.x = eWindow::WIDTH;
        else if( rect.x < 0 )
            rect.x = 0;

        m_CurImage->SetRect( rect );
        m_CurrentTime = 0.0f;
    }
}

void ShotObject::Render()
{
    if( !m_IsActive || m_Owner != GameManager::GetSingleton().GetActiveUnit().get() )
        return;
    SDL_Rect r;
    r = m_CurImage->GetRect();
    if( m_ShotType == SHOT_TYPE::RAY )
    {
        int rays = int( m_CurrentTime / m_Speed );
        if( r.w + m_CurrentTime * 10000.0f < m_Speed )
            r.w += m_CurrentTime * 10000.0f;
        else r.w = (int)m_Speed;
    }
    m_CurImage->RenderTexture( m_Flip, m_Rotation, &r );
}

void ShotObject::Reset()
{
    m_IsActive = false;
    m_CurrentTime = 0.0f;
}

void ShotObject::SetOwner( Unit* owner )
{
    m_Owner = owner;
}

void ShotObject::SetPosition( string animName )
{
    if( animName.compare( "SHOOT_UP" ) == 0 )
        m_Position = m_Positions[0];
    else if( animName.compare( "SHOOT_DOWN" ) == 0 )
        m_Position = m_Positions[2];
    else m_Position = m_Positions[1];
}

int ShotObject::GetHeightForSorting()const
{
    if( m_Owner )
    {
        if( Grid** g = m_Owner->GetGrid() )
        {
            if( m_Trajectory.y > 0 )
                return g[0]->GetMiddle().y + 5;
            if( m_CurImage )
                return m_CurImage->GetRect().y;
        }
    }
    return m_Position.y;
}