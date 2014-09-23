/*
-----------------------------------------------------------------------------
Filename:    cadogApplication.cpp
-----------------------------------------------------------------------------


This source file is generated by the
   ___                   _              __    __ _                  _
  /___\__ _ _ __ ___    /_\  _ __  _ __/ / /\ \ (_)______ _ _ __ __| |
 //  // _` | '__/ _ \  //_\\| '_ \| '_ \ \/  \/ / |_  / _` | '__/ _` |
/ \_// (_| | | |  __/ /  _  \ |_) | |_) \  /\  /| |/ / (_| | | | (_| |
\___/ \__, |_|  \___| \_/ \_/ .__/| .__/ \/  \/ |_/___\__,_|_|  \__,_|
      |___/                 |_|   |_|
	  Ogre 1.7.x Application Wizard for Code::Blocks (May 2010)
	  http://code.google.com/p/ogreappwizards/
-----------------------------------------------------------------------------
*/
#include "cadogApplication.h"
#include <cstring>

//-------------------------------------------------------------------------------------

void cadogApplication::loadScenarios()
{

    #if WIN32
    ifstream in("Maps\\slcadog.cfg");//arquivo contendo lista de cenarios!

    #else
    ifstream in("Maps/slcadog.cfg");//arquivo contendo lista de cenarios!
    #endif

	if(in.fail()){
		cerr<<"Falha ao abrir: slcadog.cfg!"<<endl;
		exit(1);
	}



    in>>mwin;
	in>>mlose;
	in>>mgover;
    in>>mosound;
    in>>mosound2;

    in>>nc;
    cenario.resize(nc);
    mcenario.resize(nc);

	for(int i=0;i<nc;i++){
        in>>cenario[i];
        in>>mcenario[i];
	}



    cc=-1;
}
//-------------------------------------------------------------------------------------
char buffer[101];

void cadogApplication::createScene(void)
{
    reset();

    cc=(cc+1)%nc;
    load(cenario[cc].c_str());

    //sprintf( buffer, "Scenario %d: %s",cc,cenario[cc].c_str());
    //mTrayMgr->showOkDialog(buffer,"Continue?");

    if(statusGame==CADOG_INIT){
        mTrayMgr->showOkDialog("Scenario tutorial","This is the tutorial scenario!");
    }

    mSoundMgr->play2D(mcenario[cc].c_str(), true);
    mDetailsPanel->show();

    int ID;

    ID=0;

    Ogre::Entity* ent;//Iterador para entidades
    Ogre::SceneNode* node;//Iterador para nos
    Ogre::Light* l;//Iterador para luzes
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

    l=mSceneMgr->createLight("Lua");
    l->setType(Ogre::Light::LT_POINT);
    l->setPosition(Ogre::Vector3(N*Dx,150,M*Dy));
    l->setDiffuseColour(1.0,1.0,2.0);
    l->setSpecularColour(1.0,1.0,2.0);

    mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 2.0));
    mSceneMgr->setShadowTechnique(Ogre::SHADOWDETAILTYPE_STENCIL);
    mSceneMgr->setFog(Ogre::FOG_EXP,Ogre::ColourValue::Black,0.001);


    Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 20000, 20000 , 10, 10, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

    ent = mSceneMgr->createEntity("GroundEntity", "ground");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
    ent->setMaterialName("Examples/GrassFloor");
    ent->setCastShadows(true);

    for(int i=0;i<N;i++)
        for(int j=0;j<M;j++)
        if(T[i][j]!='.' &&T[i][j]!='*'){
            ObjProp o=Lobjs[T[i][j]];

            sprintf(buffer,"a%d",ID);
            node=mSceneMgr->getRootSceneNode()->createChildSceneNode(buffer);

            sprintf(buffer,"b%d",ID);

            ent=mSceneMgr->createEntity(buffer,o.MashName.c_str());

            if(o.MaterialName!="null") ent->setMaterialName(o.MaterialName.c_str());


            ent->setCastShadows(true);

            node->attachObject(ent);

            Dx+=o.px; Dy+=o.py; Dz+=o.pz;

            node->scale(o.sx,o.sz,o.sy);
            node->setPosition(Ogre::Vector3(Dx*i,Dz,Dy*j));

            node->yaw(Ogre::Degree(o.yaw));
            node->roll(Ogre::Degree(o.roll));
            node->pitch(Ogre::Degree(o.pitch));

            Dx-=o.px; Dy-=o.py; Dz-=o.pz;

            if(T[i][j]=='c'){
                mcat=node;

                point u=posPlayer;
                point v=posEnemy;

                int sx=(v.x-u.x)<0?-1:1;
                int sy=(v.y-u.y)<0?-1:1;

                if(u.x==v.x) sx=0;
                if(u.y==v.y) sy=0;

                mcat->lookAt(Ogre::Vector3(10*N*Dx*sx,Dz,10*M*Dy*sy),Ogre::Node::TS_WORLD);
                mcatAc.init(ent,mcat,0);
            }else if(T[i][j]=='r'){
                mrat=node;

                sprintf(buffer,"d%d",ID);

                mcamn=mrat->createChildSceneNode(buffer);

                mcamn->attachObject(mCamera);
                mcamn->setInheritOrientation(true);

                mCamera->setPosition(Ogre::Vector3(0,120,100));
                mCamera->lookAt(Ogre::Vector3(0,-120,0));
                mCamera->yaw(Ogre::Degree(-45));
                mCamera->pitch(Ogre::Degree(-15));

                mratAc.init(ent,mrat,"Idle1");
                mrat->yaw(Ogre::Degree(45*direcao));
            }

            ID++;
        }
}

