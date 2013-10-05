#pragma once
//=====================================================================//
/*!	@file
	@brief	物理・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "btBulletDynamicsCommon.h"
#include <vector>
#include "gl_shape_drawer.hpp"
#include "gl_fw/gl_camera.hpp"

namespace bt {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	物理・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct physics {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	髪の毛を構成する構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct hair {
			float		radius_;	///< 髪の毛の半径
			vtx::fvtxs	joints_;	///< 髪の毛のジョイント
		};
		typedef std::vector<hair>	hairs;

	private:
		btDefaultCollisionConfiguration*		config_;
		btCollisionDispatcher*					dispatcher_;
		btBroadphaseInterface*					broadphase_;
		btSequentialImpulseConstraintSolver*	solver_;
		btDiscreteDynamicsWorld*				world_;

//		btAlignedObjectArray<btCollisionShape*>		shapes_;
		std::vector<btCollisionShape*>		shapes_;
//		btAlignedObjectArray<btTypedConstraint*>	joints_;
		std::vector<btTypedConstraint*>	joints_;

		int						debug_mode_;
		btVector3				sun_direction_;
		bullet::gl_shape_drawer	shape_drawer_;

		bool					use_6dof_;
		btVector3				last_picking_pos_;
		float					last_picking_dist_;
		btRigidBody*			picked_body_;
		btTypedConstraint*		pick_constraint_;

		struct param {
			float	mass_;
			float	damping_linear_;
			float	damping_rotate_;
			param() : mass_(1.0), damping_linear_(0.5f), damping_rotate_(0.5f) { }
		};
		param	current_;

		bool	render_shadow_;

		btVector3 get_ray_to_(const gl::camera& cam, int x, int y);
		void picking_body_(const gl::camera& cam, const btVector3& ray);
		void moveing_body_(const gl::camera& cam, const btVector3& ray);
		void remove_picking_constraint_();
		void render_world_(int pass);
		btRigidBody* new_body_(btCollisionShape* shape, const vtx::fvtx& pos, const vtx::fvtx& dir);
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		physics() : config_(0), dispatcher_(0), broadphase_(0), solver_(0), world_(0),
			debug_mode_(0), sun_direction_(btVector3(1.0f, -2.0f, 1.0f) * 1000.0f),
			use_6dof_(false), last_picking_pos_(0.0f, 0.0f, 0.0f), last_picking_dist_(0.0f),
			picked_body_(0), pick_constraint_(0),
			current_(),
			render_shadow_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~physics() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		void set_current_mass(float mass) { current_.mass_ = mass; }


		//-----------------------------------------------------------------//
		/*!
			@brief	地面剛体生成
		*/
		//-----------------------------------------------------------------//
		void create_ground_body();


		//-----------------------------------------------------------------//
		/*!
			@brief	箱作成
			@param[in]	org	起点
			@param[in]	size	サイズ
			@param[in]	dir		方向（省略するとZ上向き）
			@return RigidBody のポインター
		*/
		//-----------------------------------------------------------------//
		btRigidBody* create_box(const vtx::fvtx& org, const vtx::fvtx& size,
			const vtx::fvtx& dir = vtx::fvtx(0.0f, 0.0f, 1.0f));


		//-----------------------------------------------------------------//
		/*!
			@brief	球作成
			@param[in]	org	起点
			@param[in]	radius	半径
			@return RigidBody のポインター
		*/
		//-----------------------------------------------------------------//
		btRigidBody* create_sphere(const vtx::fvtx& org, float radius);


		//-----------------------------------------------------------------//
		/*!
			@brief	シリンダー作成
			@param[in]	org	起点
			@param[in]	end	終点
			@param[in]	radius	半径
			@return RigidBody のポインター
		*/
		//-----------------------------------------------------------------//
		btRigidBody* create_cylinder(const vtx::fvtx& org, const vtx::fvtx& end, float radius);


		//-----------------------------------------------------------------//
		/*!
			@brief	カプセル作成
			@param[in]	org	起点
			@param[in]	end	終点
			@param[in]	radius	半径
			@return RigidBody のポインター
		*/
		//-----------------------------------------------------------------//
		btRigidBody* create_capsule(const vtx::fvtx& org, const vtx::fvtx& end, float radius);


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
		btTypedConstraint* create_joint(btRigidBody* bdya, btRigidBody* bdyb, float ofsa, float ofsb);


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
		btTypedConstraint* create_spring_joint(btRigidBody* bdya, btRigidBody* bdyb, float ofsa, float ofsb);


		//-----------------------------------------------------------------//
		/*!
			@brief	剛体生成
			@param[in]	hs	髪の毛構造
		*/
		//-----------------------------------------------------------------//
		void create_body(const hairs& hs);


		//-----------------------------------------------------------------//
		/*!
			@brief	シュミレーション
		*/
		//-----------------------------------------------------------------//
		void update(const gl::camera& cam);


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render();


		//-----------------------------------------------------------------//
		/*!
			@brief	剛体廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy_body();


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();


	};

}
