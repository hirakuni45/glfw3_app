//=====================================================================//
/*!	@file
	@brief	物理関係・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "physics.hpp"
#include <boost/foreach.hpp>
#include "gl_fw/IGLcore.hpp"
#include <boost/foreach.hpp>
#include "gl_fw/quat.hpp"

namespace bt {

	btVector3 physics::get_ray_to_(const gl::camera& cam, int x, int y)
	{
#if 0
		if(m_ortho) {
			btScalar aspect;
			btVector3 extents;
			aspect = m_glutScreenWidth / (btScalar)m_glutScreenHeight;
			extents.setValue(aspect * 1.0f, 1.0f,0);
		
			extents *= m_cameraDistance;
			btVector3 lower = m_cameraTargetPosition - extents;
			btVector3 upper = m_cameraTargetPosition + extents;

			btScalar u = x / btScalar(m_glutScreenWidth);
			btScalar v = (m_glutScreenHeight - y) / btScalar(m_glutScreenHeight);
		
			btVector3	p(0,0,0);
			p.setValue((1.0f - u) * lower.getX() + u * upper.getX(),(1.0f - v) * lower.getY() + v * upper.getY(),m_cameraTargetPosition.getZ());
			return p;
		}
#endif
		const vtx::fvtx& v = cam.get_eye();
		btVector3 rayFrom(v.x, v.y, v.z);
		vtx::fvtx d = cam.get_target() - cam.get_eye();
		btVector3 rayForward(d.x, d.y, d.z);
		rayForward.normalize();
		float farPlane = cam.get_far();
		rayForward *= farPlane;

		const vtx::fvtx& up = cam.get_up();
		btVector3 horizontal = rayForward.cross(btVector3(up.x, up.y, up.z));
		horizontal.normalize();
		btVector3 vertical = horizontal.cross(rayForward);
		vertical.normalize();

		float tanfov = tanf(0.5f * cam.get_fov() * vtx::g_deg2rad_f);
		horizontal *= 2.0f * farPlane * tanfov;
		vertical   *= 2.0f * farPlane * tanfov;

		btScalar aspect(cam.get_aspect());

		horizontal *= aspect;

		btVector3 rayToCenter = rayFrom + rayForward;
		const vtx::fpos& size = cam.get_size();
		btVector3 dH = horizontal * 1.0f / size.x;
		btVector3 dV = vertical   * 1.0f / size.y;

		btVector3 rayTo = rayToCenter - 0.5f * horizontal + 0.5f * vertical;
		rayTo += btScalar(x) * dH;
		rayTo -= btScalar(y) * dV;

		return rayTo;
	}


	void physics::picking_body_(const gl::camera& cam, const btVector3& ray_to)
	{					
		btVector3 ray_from;
		const vtx::fvtx& cp = cam.get_eye();
		ray_from = btVector3(cp.x, cp.y, cp.z);

		btCollisionWorld::ClosestRayResultCallback ray_callback(ray_from, ray_to);
		world_->rayTest(ray_from, ray_to, ray_callback);
		if(ray_callback.hasHit()) {
			btRigidBody* body = btRigidBody::upcast(ray_callback.m_collisionObject);
			if(body) {
				//other exclusions?
				if(!(body->isStaticObject() || body->isKinematicObject())) {
					picked_body_ = body;
					body->setActivationState(DISABLE_DEACTIVATION);
					const btVector3& pick_pos = ray_callback.m_hitPointWorld;
					// printf("pickPos=%f,%f,%f\n",pickPos.getX(),pickPos.getY(),pickPos.getZ());
					const btVector3& local_pivot = body->getCenterOfMassTransform().inverse() * pick_pos;
					if(use_6dof_) {
						btTransform tr;
						tr.setIdentity();
						tr.setOrigin(local_pivot);
						btGeneric6DofConstraint* dof = new btGeneric6DofConstraint(*body, tr, false);
						dof->setLinearLowerLimit(btVector3(0, 0, 0));
						dof->setLinearUpperLimit(btVector3(0, 0, 0));
						dof->setAngularLowerLimit(btVector3(0, 0, 0));
						dof->setAngularUpperLimit(btVector3(0, 0, 0));
						world_->addConstraint(dof);
						pick_constraint_ = dof;
						for(int i = 0; i < 6; ++i) {
							dof->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, i);
							dof->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, i);
						}
					} else {
						btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body, local_pivot);
						world_->addConstraint(p2p);
						pick_constraint_ = p2p;
///						p2p->m_setting.m_impulseClamp = mousePickClamping;
						//very weak constraint for picking
						p2p->m_setting.m_tau = 0.001f;
/*
						p2p->setParam(BT_CONSTRAINT_CFM,0.8,0);
						p2p->setParam(BT_CONSTRAINT_CFM,0.8,1);
						p2p->setParam(BT_CONSTRAINT_CFM,0.8,2);
						p2p->setParam(BT_CONSTRAINT_ERP,0.1,0);
						p2p->setParam(BT_CONSTRAINT_ERP,0.1,1);
						p2p->setParam(BT_CONSTRAINT_ERP,0.1,2);
						*/
					}
					use_6dof_ = !use_6dof_;
					//save mouse position for dragging
					last_picking_pos_ = ray_to;
///					gHitPos = pick_pos;
					last_picking_dist_ = (pick_pos - ray_from).length();
				}
			}
		}
	}


	void physics::moveing_body_(const gl::camera& cam, const btVector3& ray)
	{
		if(pick_constraint_) {
			//move the constraint pivot
			if(pick_constraint_->getConstraintType() == D6_CONSTRAINT_TYPE) {
				btGeneric6DofConstraint* pick = static_cast<btGeneric6DofConstraint*>(pick_constraint_);
				if(pick) {
					//keep it at the same picking distance
					btVector3 new_pivot;
//					if (m_ortho)
//					{
//						const btVector3& old_pivot = pick->getFrameOffsetA().getOrigin();
//						newPivot = old_pivot;
//						newPivot.setX(ray.getX());
//						newPivot.setY(ray.getY());
//					} else
					{
						const vtx::fvtx& v = cam.get_eye();
						btVector3 ray_from(v.x, v.y, v.z);
						btVector3 dir = ray - ray_from;
						dir.normalize();
						dir *= last_picking_dist_;
						new_pivot = ray_from + dir;
					}
					pick->getFrameOffsetA().setOrigin(new_pivot);
				}
			} else {
				btPoint2PointConstraint* pick = static_cast<btPoint2PointConstraint*>(pick_constraint_);
				if(pick) {
					//keep it at the same picking distance
					btVector3 new_pivot;
//					if (m_ortho)
//					{
//						btVector3 oldPivotInB = pick->getPivotInB();
//						newPivotB = oldPivotInB;
//						newPivotB.setX(ray.getX());
//						newPivotB.setY(ray.getY());
//					} else
					{
						const vtx::fvtx& v = cam.get_eye();
						btVector3 ray_from(v.x, v.y, v.z);
						btVector3 dir = ray - ray_from;
						dir.normalize();
						dir *= last_picking_dist_;
						new_pivot = ray_from + dir;
					}
					pick->setPivotB(new_pivot);
				}
			}
		}
	}


	void physics::remove_picking_constraint_()
	{
		if(world_ && picked_body_ && pick_constraint_) {
			world_->removeConstraint(pick_constraint_);
			delete pick_constraint_;
			pick_constraint_ = 0;

			picked_body_->forceActivationState(ACTIVE_TAG);
			picked_body_->setDeactivationTime(0.0f);
			picked_body_ = 0;
		}
	}


	void physics::render_world_(int pass)
	{
		btScalar m[16];
		btMatrix3x3 rot;
		rot.setIdentity();
		int num = world_->getNumCollisionObjects();
		btVector3 wireColor(1, 0, 0);
		for(int i = 0; i < num; ++i) {
			btCollisionObject* cobj = world_->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(cobj);
			if(body && body->getMotionState()) {
				btDefaultMotionState* state = (btDefaultMotionState*)body->getMotionState();
				state->m_graphicsWorldTrans.getOpenGLMatrix(m);
				rot = state->m_graphicsWorldTrans.getBasis();
			} else {
				cobj->getWorldTransform().getOpenGLMatrix(m);
				rot = cobj->getWorldTransform().getBasis();
			}

			btVector3 wireColor(1.0f, 1.0f, 0.5f); //wants deactivation
			if(i & 1) wireColor = btVector3(0.0f, 0.0f, 1.0f);

			// color differently for active, sleeping, wantsdeactivation states
			if(cobj->getActivationState() == 1) {
				if (i & 1) {
					wireColor += btVector3 (1.f,0.f,0.f);
				} else {			
					wireColor += btVector3 (.5f,0.f,0.f);
				}
			}
			if(cobj->getActivationState() == 2) {	//ISLAND_SLEEPING
				if(i & 1) {
					wireColor += btVector3 (0.f,1.f, 0.f);
				} else {
					wireColor += btVector3 (0.f,0.5f,0.f);
				}
			}

			btVector3 aabbMin,aabbMax;
			world_->getBroadphase()->getBroadphaseAabb(aabbMin, aabbMax);		
			aabbMin -= btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);
			aabbMax += btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);

///			printf("aabbMin=(%f,%f,%f)\n",aabbMin.getX(),aabbMin.getY(),aabbMin.getZ());
///			printf("aabbMax=(%f,%f,%f)\n",aabbMax.getX(),aabbMax.getY(),aabbMax.getZ());
///			world_->getDebugDrawer()->drawAabb(aabbMin,aabbMax,btVector3(1,1,1));

			if (!(debug_mode_ & btIDebugDraw::DBG_DrawWireframe)) {
				switch(pass)
				{
				case 0:
					shape_drawer_.drawOpenGL(m, cobj->getCollisionShape(),
						wireColor, debug_mode_, aabbMin, aabbMax);
					break;
				case 1:
					shape_drawer_.drawShadow(m, sun_direction_ * rot,
						cobj->getCollisionShape(), aabbMin, aabbMax);
					break;
				case 2:
					shape_drawer_.drawOpenGL(m, cobj->getCollisionShape(),
						wireColor * btScalar(0.3f), 0, aabbMin, aabbMax);
					break;
				}
			}
		}
	}


	btRigidBody* physics::new_body_(btCollisionShape* shape, const vtx::fvtx& pos, const vtx::fvtx& dir)
	{
		btTransform transform;
		transform.setIdentity();

		btScalar mass(current_.mass_);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 inertia(0, 0, 0);
		if (isDynamic) {
			shape->calculateLocalInertia(mass, inertia);
		}
		transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
		qtx::fquat qu;
		qu.look_rotation(dir, vtx::fvtx(0.0f, 0.0f, 1.0f));
		btQuaternion q(btScalar(qu.x), btScalar(qu.y), btScalar(qu.z), btScalar(qu.t));
		transform.setRotation(q);
		
		// using motionstate is recommended,
		// it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* state = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo info(mass, state, shape, inertia);
		return new btRigidBody(info);
	}



	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void physics::initialize()
	{
		shape_drawer_.enable_texture();

		config_ = new btDefaultCollisionConfiguration();
		dispatcher_ = new	btCollisionDispatcher(config_);
		broadphase_ = new btDbvtBroadphase();
		solver_ = new btSequentialImpulseConstraintSolver;
		world_ = new btDiscreteDynamicsWorld(dispatcher_, broadphase_, solver_, config_);

		world_->setGravity(btVector3(0.0f, 0.0f, -9.8f));
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	地面剛体生成
	*/
	//-----------------------------------------------------------------//
	void physics::create_ground_body()
	{
		current_.mass_ = 0.0f;	/// 重さ０（固定）
		create_box(vtx::fvtx(0.0f, 0.0f, -2.5f), vtx::fvtx(7.5f, 7.5f, 0.05f));

		current_.mass_ = 1.0f;
		create_sphere(vtx::fvtx(0.0f, 0.0f, 10.0f), 0.5f);

		// 実験用剛体～
		if(0) {
			btRigidBody* bodyA;
			btRigidBody* bodyB;
			vtx::fvtx posA(0.0f, 0.0f, 1.0f);
			vtx::fvtx posB(0.0f, 0.0f, 1.0f + 2.0f + 0.2f * 2);
			{
				current_.mass_ = 0.0f;
				btCollisionShape* cshape = new btCapsuleShapeZ(0.2f, 2.0f);
				shapes_.push_back(cshape);
				vtx::fvtx dir(0.0f, 0.0f, 1.0f);
				bodyA = new_body_(cshape, posA, dir);
				bodyA->setDamping(0.15f, 0.15f);
				world_->addRigidBody(bodyA);
			}
			{
				current_.mass_ = 1.0f;
				btCollisionShape* cshape = new btCapsuleShapeZ(0.2f, 2.0f);
				shapes_.push_back(cshape);
				vtx::fvtx dir(0.0f, 0.0f, 1.0f);
				bodyB = new_body_(cshape, posB, dir);
				bodyB->setDamping(0.15f, 0.15f);
				world_->addRigidBody(bodyB);
			}

			create_joint(bodyA, bodyB, 1.2f, -1.2f);
		}

	}


	//-----------------------------------------------------------------//
	/*!
		@brief	箱作成
		@param[in]	org		起点
		@param[in]	size	サイズ
		@param[in]	dir		方向（省略するとZ上向き）
		@return RigidBody のポインター
	*/
	//-----------------------------------------------------------------//
	btRigidBody* physics::create_box(const vtx::fvtx& org, const vtx::fvtx& size, const vtx::fvtx& dir)
	{
		btCollisionShape* cshape = new btBoxShape(btVector3(size.x, size.y, size.z));
		shapes_.push_back(cshape);
		btRigidBody* body = new_body_(cshape, org, dir);
		body->setDamping(current_.damping_linear_, current_.damping_rotate_);
		world_->addRigidBody(body);
		return body;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	球作成
		@param[in]	org	起点
		@param[in]	radius	半径
		@return RigidBody のポインター
	*/
	//-----------------------------------------------------------------//
	btRigidBody* physics::create_sphere(const vtx::fvtx& org, float radius)
	{
		btCollisionShape* cshape = new btSphereShape(radius);
		shapes_.push_back(cshape);
		btRigidBody* body = new_body_(cshape, org, vtx::fvtx(0.0f, 0.0f, 1.0f));
		body->setDamping(current_.damping_linear_, current_.damping_rotate_);
		world_->addRigidBody(body);
		return body;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	シリンダー作成
		@param[in]	org	起点
		@param[in]	end	終点
		@param[in]	radius	半径
		@return RigidBody のポインター
	*/
	//-----------------------------------------------------------------//
	btRigidBody* physics::create_cylinder(const vtx::fvtx& org, const vtx::fvtx& end, float radius)
	{
		vtx::fvtx d = end - org;
		vtx::fvtx n;
		if(vtx::normalize(d, n)) {
			float len = d.len() * 0.5f;
			btVector3 extent(radius, radius, len);
			btCollisionShape* cshape = new btCylinderShapeZ(extent);
			shapes_.push_back(cshape);
			vtx::fvtx p = (org + end) * 0.5f;	// 中心
			btRigidBody* body = new_body_(cshape, p, n);
			body->setDamping(current_.damping_linear_, current_.damping_rotate_);
			world_->addRigidBody(body);
			return body;
		} else {
			return 0;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	カプセル作成
		@param[in]	org	起点
		@param[in]	end	終点
		@param[in]	radius	半径
		@param[in]	shave	ジョイント部分を削る場合「true」
		@return btRigidBody のポインター
	*/
	//-----------------------------------------------------------------//
	btRigidBody* physics::create_capsule(const vtx::fvtx& org, const vtx::fvtx& end, float radius)
	{
		vtx::fvtx d = end - org;
		vtx::fvtx n;
		if(vtx::normalize(d, n)) {
			float len = d.len();
			len -= radius * 2;
			btCollisionShape* cshape = new btCapsuleShapeZ(radius, len);
			shapes_.push_back(cshape);
			vtx::fvtx p = (org + end) * 0.5f;	// 中心
			btRigidBody* body = new_body_(cshape, p, n);
			body->setDamping(current_.damping_linear_, current_.damping_rotate_);
			world_->addRigidBody(body);
			return body;
		} else {
			return 0;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ジョイント作成
		@param[in]	bdya	ボディーＡ
		@param[in]	bdyb	ボディーＢ
		@param[in]	ofsa	ボディーＡオフセット
		@param[in]	ofsb	ボディーＢオフセット
		@return btJoint のポインター
	*/
	//-----------------------------------------------------------------//
	btTypedConstraint* physics::create_joint(btRigidBody* bdya, btRigidBody* bdyb, float ofsa, float ofsb)
	{
		bool useLinearReferenceFrameA = true;
		btTransform trans_a;
		trans_a.setIdentity();
		trans_a.setOrigin(btVector3(btScalar(0.0f), btScalar(0.0f), btScalar(ofsa)));
		btTransform trans_b;
		trans_b.setIdentity();
		trans_b.setOrigin(btVector3(btScalar(0.0f), btScalar(0.0f), btScalar(ofsb)));
		btGeneric6DofConstraint* joint = new btGeneric6DofConstraint(*bdya, *bdyb, trans_a, trans_b,
			useLinearReferenceFrameA);

//		joint->setAngularLowerLimit(btVector3(-SIMD_PI*0.5f,-SIMD_EPSILON,-SIMD_PI*0.5f));
//		joint->setAngularUpperLimit(btVector3( SIMD_PI*0.5f, SIMD_EPSILON, SIMD_PI*0.5f));
		joint->setAngularLowerLimit(btVector3(-SIMD_PI * 0.5f,-SIMD_PI * 0.5f, -SIMD_PI * 0.5f));
		joint->setAngularUpperLimit(btVector3( SIMD_PI * 0.5f, SIMD_PI * 0.5f,  SIMD_PI * 0.5f));

		world_->addConstraint(joint, true);
		joints_.push_back(joint);

		return joint;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	スプリング・ジョイント作成
		@param[in]	bdya	ボディーＡ
		@param[in]	bdyb	ボディーＢ
		@param[in]	ofsa	ボディーＡオフセット
		@param[in]	ofsb	ボディーＢオフセット
		@return btJoint のポインター
	*/
	//-----------------------------------------------------------------//
	btTypedConstraint* physics::create_spring_joint(btRigidBody* bdya, btRigidBody* bdyb, float ofsa, float ofsb)
	{
		bool useLinearReferenceFrameA = true;
		btTransform trans_a;
		trans_a.setIdentity();
		trans_a.setOrigin(btVector3(btScalar(0.0f), btScalar(0.0f), btScalar(ofsa)));
		btTransform trans_b;
		trans_b.setIdentity();
		trans_b.setOrigin(btVector3(btScalar(0.0f), btScalar(0.0f), btScalar(ofsb)));
		btGeneric6DofSpringConstraint* joint = new btGeneric6DofSpringConstraint(*bdya, *bdyb,
			trans_a, trans_b, useLinearReferenceFrameA);

		// 初期位置をバネの復元ポイントとする
		joint->setEquilibriumPoint();

//		joint->enableSpring(0, true);	/// X
//		joint->enableSpring(1, true);	/// Y
//		joint->enableSpring(2, true);	/// Z
		joint->enableSpring(3, true);	/// X
		joint->enableSpring(4, true);	/// Y
		joint->enableSpring(5, true);	/// Z

		// バネの柔らかさ
//		joint->setStiffness(0, 100.0f);	/// X
//		joint->setStiffness(1, 100.0f);	/// Y
//		joint->setStiffness(2, 100.0f);	/// Z
		joint->setStiffness(3, 10.0f);	/// X
		joint->setStiffness(4, 10.0f);	/// Y
		joint->setStiffness(5, 10.0f);	/// Z


		/// 減衰率
		joint->setDamping(0, 0.8f);
		joint->setDamping(1, 0.8f);
		joint->setDamping(2, 0.8f);
		joint->setDamping(3, 0.8f);
		joint->setDamping(4, 0.8f);
		joint->setDamping(5, 0.8f);

		// バネの場合はリミッターを無効にする！
		joint->setAngularLowerLimit(btVector3(1.0f, 1.0f, 1.0f));
		joint->setAngularUpperLimit(btVector3(-1.0f, -1.0f, -1.0f));

		world_->addConstraint(joint, true);
		joints_.push_back(joint);

		return joint;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	剛体生成
		@param[in]	hs	髪の毛構造
	*/
	//-----------------------------------------------------------------//
	void physics::create_body(const hairs& hs)
	{
		BOOST_FOREACH(const hair& h, hs) {
			if(!h.joints_.empty()) {
				vtx::fvtx org = h.joints_[0];
				BOOST_FOREACH(const vtx::fvtx& v, h.joints_) {
					vtx::fvtx d = v - org;
					vtx::fvtx n;
					if(vtx::normalize(d, n)) {
						btVector3 extent(h.radius_, h.radius_, d.len());
						btCollisionShape* cshape = new btCylinderShape(extent);
						shapes_.push_back(cshape);
						vtx::fvtx p = (org + v) * 0.5f;	// 中心
						btRigidBody* body = new_body_(cshape, p, n);
						body->setDamping(0.15f, 0.85f);
						world_->addRigidBody(body);
					}
					org = v;
				}
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	シュミレーション
	*/
	//-----------------------------------------------------------------//
	void physics::update(const gl::camera& cam)
	{
		world_->stepSimulation(1.0f / 60.0f, 10);

		for(int j = world_->getNumCollisionObjects() - 1; j >= 0; --j) {
			btCollisionObject* obj = world_->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
			if(body && body->getMotionState()) {
				btTransform trans;
				body->getMotionState()->getWorldTransform(trans);
			}
		}

		if(!cam.get_touch()) {
			using namespace gl;
			IGLcore* igl = get_glcore();
			if(igl == 0) return;
			const gldev& dev = igl->get_device();

			const vtx::spos& mspos = dev.get_mouse_position();
			btVector3 ray_to = get_ray_to_(cam, mspos.x, mspos.y);

			if(dev.get_mouse_positive() & MOUSE_LEFT) {
				picking_body_(cam, ray_to);
			}
			if(dev.get_mouse_level() & MOUSE_LEFT) {
				moveing_body_(cam, ray_to);
			} else {
				remove_picking_constraint_();
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void physics::render()
	{
		if(render_shadow_) {
			glClear(GL_STENCIL_BUFFER_BIT);
			glEnable(GL_CULL_FACE);
			render_world_(0);

			glDisable(GL_LIGHTING);
			glDepthMask(GL_FALSE);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_STENCIL_TEST);
			glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
			glStencilFunc(GL_ALWAYS,1,0xFFFFFFFFL);
			glFrontFace(GL_CCW);
			glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
			render_world_(1);
			glFrontFace(GL_CW);
			glStencilOp(GL_KEEP,GL_KEEP,GL_DECR);
			render_world_(1);
			glFrontFace(GL_CCW);

			glPolygonMode(GL_FRONT,GL_FILL);
			glPolygonMode(GL_BACK,GL_FILL);
			glShadeModel(GL_SMOOTH);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glEnable(GL_LIGHTING);
			glDepthMask(GL_TRUE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
			glEnable(GL_CULL_FACE);
			glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

			glDepthFunc(GL_LEQUAL);
			glStencilFunc( GL_NOTEQUAL, 0, 0xFFFFFFFFL );
			glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
			glDisable(GL_LIGHTING);
			render_world_(2);
			glEnable(GL_LIGHTING);
			glDepthFunc(GL_LESS);
			glDisable(GL_STENCIL_TEST);
			glDisable(GL_CULL_FACE);
		} else {
			glDisable(GL_CULL_FACE);
			render_world_(0);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	剛体廃棄
	*/
	//-----------------------------------------------------------------//
	void physics::destroy_body()
	{
		for(int i = 0; i < shapes_.size(); ++i) {
			btCollisionShape* shape = shapes_[i];
			delete shape;
		}
		shapes_.clear();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void physics::destroy()
	{
		delete world_;
		world_ = 0;
		delete solver_;
		solver_ = 0;
		delete broadphase_;
		broadphase_ = 0;
		delete dispatcher_;
		dispatcher_ = 0;
		delete config_;
		config_ = 0;
	}


}
