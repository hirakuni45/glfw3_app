
#ifndef	__EFFEKSEER_LOADER_H__
#define	__EFFEKSEER_LOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	設定クラス
	@note
	EffectLoader等、ファイル読み込みに関する設定することができる。
	Managerの代わりにエフェクト読み込み時に使用することで、Managerとは独立してEffectインスタンスを生成することができる。
*/
	class Setting
	{
	private:
		int32_t		m_ref;

		/* 座標系 */
		eCoordinateSystem		m_coordinateSystem;

		EffectLoader*	m_effectLoader;
		TextureLoader*	m_textureLoader;
		SoundLoader*	m_soundLoader;
		ModelLoader*	m_modelLoader;

		/**
			@brief	コンストラクタ
			*/
		Setting();

		/**
			@brief	デストラクタ
			*/
		 ~Setting();
	public:

		/**
			@brief	設定インスタンスを生成する。
		*/
		static Setting* Create();

		/**
			@brief	参照カウンタを加算する。
			@return	参照カウンタ
		*/
		int32_t AddRef();

		/**
			@brief	参照カウンタを減算する。
			@return	参照カウンタ
		*/
		int32_t Release();

		/**
		@brief	座標系を取得する。
		@return	座標系
		*/
		eCoordinateSystem GetCoordinateSystem() const;

		/**
		@brief	座標系を設定する。
		@param	coordinateSystem	[in]	座標系
		@note
		座標系を設定する。
		エフェクトファイルを読み込む前に設定する必要がある。
		*/
		void SetCoordinateSystem(eCoordinateSystem coordinateSystem);

		/**
			@brief	エフェクトローダーを取得する。
			@return	エフェクトローダー
			*/
		EffectLoader* GetEffectLoader();

		/**
			@brief	エフェクトローダーを設定する。
			@param	loader	[in]		ローダー
			*/
		void SetEffectLoader(EffectLoader* loader);

		/**
			@brief	テクスチャローダーを取得する。
			@return	テクスチャローダー
			*/
		TextureLoader* GetTextureLoader();

		/**
			@brief	テクスチャローダーを設定する。
			@param	loader	[in]		ローダー
			*/
		void SetTextureLoader(TextureLoader* loader);

		/**
			@brief	モデルローダーを取得する。
			@return	モデルローダー
			*/
		ModelLoader* GetModelLoader();

		/**
			@brief	モデルローダーを設定する。
			@param	loader	[in]		ローダー
			*/
		void SetModelLoader(ModelLoader* loader);

		/**
			@brief	サウンドローダーを取得する。
			@return	サウンドローダー
			*/
		SoundLoader* GetSoundLoader();

		/**
			@brief	サウンドローダーを設定する。
			@param	loader	[in]		ローダー
			*/
		void SetSoundLoader(SoundLoader* loader);

	};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_LOADER_H__
