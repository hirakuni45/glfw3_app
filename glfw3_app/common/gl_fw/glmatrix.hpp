#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenGL �}�g���b�N�X�E�G�~�����[�^�[�i�w�b�_�[�j
	@author	�����M�m (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stack>
#include "utils/vtx.hpp"
#include "utils/mtx.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	OpenGL matrix �G�~�����[�^�[�N���X
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename T>
	struct matrix {

		typedef T	value_type;
		typedef mtx::matrix4<T> matrix_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	�}�g���b�N�X�E���[�h
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct mode {
			enum type {
				modelview,
				projection,
				num_
			};
		};

	private:
		typename mode::type		mode_;

		matrix_type				acc_[mode::num_];
		std::stack<matrix_type>	stack_[mode::num_];

		T			near_;
		T			far_;

		int			vp_x_;
		int			vp_y_;
		int			vp_w_;
		int			vp_h_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �n�}�g���b�N�X����R���X�g���N�^�[
		 */
		//-----------------------------------------------------------------//
		matrix() : mode_(mode::modelview),
				   near_(0.0f), far_(1.0f),
				   vp_x_(0), vp_y_(0), vp_w_(0), vp_h_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �n�}�g���b�N�X����f�X�g���N�^�[
		 */
		//-----------------------------------------------------------------//
		~matrix() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �}�g���b�N�X���[�h��ݒ�
			@param[in]	mode	�}�g���b�N�X�E���[�h
		 */
		//-----------------------------------------------------------------//
		void mode(typename mode::type mode) { mode_ = mode; }


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �}�g���b�N�X���[�h���擾
			@return �}�g���b�N�X���[�h
		 */
		//-----------------------------------------------------------------//
		typename mode::type get_mode() const { return mode_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �r���[�|�[�g�����o���B
			@param[in]	x	X ���̈ʒu
			@param[in]	y	Y ���̈ʒu
			@param[in]	w	X ���̕�
			@param[in]	h	Y ���̍���
		 */
		//-----------------------------------------------------------------//
		void get_viewport(int& x, int& y, int& w, int& h) const {
			x = vp_x_;
			y = vp_y_;
			w = vp_w_;
			h = vp_h_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �r���[�|�[�g�̐ݒ�
			@param[in]	x	X ���̈ʒu
			@param[in]	y	Y ���̈ʒu
			@param[in]	w	X ���̕�
			@param[in]	h	Y ���̍���
		 */
		//-----------------------------------------------------------------//
		void set_viewport(int x, int y, int w, int h) {
			vp_x_ = x;
			vp_y_ = y;
			vp_w_ = w;
			vp_h_ = h;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �J�����g�E�}�g���b�N�X�ɒP�ʍs����Z�b�g
		 */
		//-----------------------------------------------------------------//
		void identity() { acc_[mode_].identity(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	�J�����g�E�}�g���b�N�X�Ƀ��[�h
			@param[in]	m	�}�g���b�N�X�̃|�C���^�[�擪�ifmat4�j
		 */
		//-----------------------------------------------------------------//
		void load(const value_type& m) { acc_[mode_] = m; }


		//-----------------------------------------------------------------//
		/*!
			@brief	�J�����g�E�}�g���b�N�X�Ƀ��[�h
			@param[in]	m	�}�g���b�N�X�̃|�C���^�[�擪�ifloat�j
		 */
		//-----------------------------------------------------------------//
		void load(const T* m) { acc_[mode_] = m; }


		//-----------------------------------------------------------------//
		/*!
			@brief	�J�����g�E�}�g���b�N�X�Ƀ��[�h
			@param[in]	m	�}�g���b�N�X�̃|�C���^�[�擪�idouble�j
		 */
		//-----------------------------------------------------------------//
		void load(const double* m) { acc_[mode_] = m; }


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL 4 X 4 �s����J�����g�E�}�g���b�N�X�ƐώZ
			@param[in]	m	4 X 4 �}�g���b�N�X
		 */
		//-----------------------------------------------------------------//
		void mult(const mtx::matrix4<T>& m) {
			mtx::matmul4<T>(acc_[mode_].m, acc_[mode_].m, m.m);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL 4 X 4 �s����J�����g�E�}�g���b�N�X�ƐώZ
			@param[in]	m	�}�g���b�N�X��ifloat�j
		 */
		//-----------------------------------------------------------------//
		void mult(const float* m) {
			mtx::matrix4<T> tm = m;
			mtx::matmul4<T>(acc_[mode_].m, acc_[mode_].m, tm.m);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL 4 X 4 �s����J�����g�E�}�g���b�N�X�ƐώZ
			@param[in]	m	�}�g���b�N�X��idouble�j
		 */
		//-----------------------------------------------------------------//
		void mult(const double* m) {
			mtx::matrix4<T> tm = m;
			mtx::matmul4<T>(acc_[mode_].m, acc_[mode_].m, tm.m);
		}

		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �J�����g�E�}�g���b�N�X���X�^�b�N�ɑޔ�
		 */
		//-----------------------------------------------------------------//
		void push() { stack_[mode_].push(acc_[mode_]); }


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �J�����g�E�}�g���b�N�X���X�^�b�N���畜�A
		 */
		//-----------------------------------------------------------------//
		void pop() { stack_[mode_].pop(); acc_[mode_] = stack_[mode_].top(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL ���̐ύs����J�����g�E�}�g���b�N�X�ɍ�������
			@param[in]	left	�N���b�v���ʏ�̈ʒu�i���j
			@param[in]	right	�N���b�v���ʏ�̈ʒu�i�E�j
			@param[in]	bottom	�N���b�v���ʏ�̈ʒu�i���j
			@param[in]	top		�N���b�v���ʏ�̈ʒu�i��j
			@param[in]	nearval	�N���b�v���ʏ�̈ʒu�i��O�j
			@param[in]	farval	�N���b�v���ʏ�̈ʒu�i���j
		 */
		//-----------------------------------------------------------------//
		void frustum(T left, T right, T bottom, T top, T nearval, T farval) {
			near_ = nearval;
			far_ = farval;
			acc_[mode_].frustum(left, right, bottom, top, nearval, farval);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL ���ˉe�s����J�����g�E�}�g���b�N�X�ɍ�������
			@param[in]	left	�N���b�v���ʏ�̈ʒu�i���j
			@param[in]	right	�N���b�v���ʏ�̈ʒu�i�E�j
			@param[in]	bottom	�N���b�v���ʏ�̈ʒu�i���j
			@param[in]	top		�N���b�v���ʏ�̈ʒu�i��j
			@param[in]	nearval	�N���b�v���ʏ�̈ʒu�i��O�j
			@param[in]	farval	�N���b�v���ʏ�̈ʒu�i���j
		 */
		//-----------------------------------------------------------------//
		void ortho(T left, T right, T bottom, T top, T nearval, T farval) {
			near_ = nearval;
			far_  = farval;
			acc_[mode_].ortho(left, right, bottom, top, nearval, farval);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL/GLU gluPerspective �Ɠ����ȍs����J�����g�E�}�g���b�N�X�ɍ�������
			@param[in]	fovy	����p�x
			@param[in]	aspect	�A�X�y�N�g��
			@param[in]	nearval	�N���b�v���ʏ�̈ʒu�i��O�j
			@param[in]	farval	�N���b�v���ʏ�̈ʒu�i���j
		 */
		//-----------------------------------------------------------------//
		void perspective(T fovy, T aspect, T nearval, T farval) {
			near_ = nearval;
			far_  = farval;
			acc_[mode_].perspective(fovy, aspect, nearval, farval);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL/GLU gluLookAt �Ɠ����ȍs����J�����g�E�}�g���b�N�X�ɍ�������
			@param[in]	eye �J�����̈ʒu
			@param[in]	center ��������
			@param[in]	up �J�����̏���������x�N�g��
		 */
		//-----------------------------------------------------------------//
		void look_at(const vtx::vertex3<T>& eye, const vtx::vertex3<T>& center, const vtx::vertex3<T>& up) {
			acc_[mode_].look_at(eye, center, up);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �X�P�[��
			@param[in]	v	�X�P�[��
		 */
		//-----------------------------------------------------------------//
		void scale(const vtx::vertex3<T>& v) { acc_[mode_].scale(v); }


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �X�P�[��
			@param[in]	x	X �X�P�[��
			@param[in]	y	Y �X�P�[��
			@param[in]	z	Z �X�P�[��
		 */
		//-----------------------------------------------------------------//
		void scale(T x, T y, T z) { acc_[mode_].scale(vtx::vertex3<T>(x, y, z)); }
 

		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL �ړ��s����J�����g�E�}�g���b�N�X�ɍ�������
			@param[in]	x	X ���ړ���
			@param[in]	y	Y ���ړ���
			@param[in]	z	Z ���ړ���
		 */
		//-----------------------------------------------------------------//
		void translate(T x, T y, T z) {
			acc_[mode_].translate(vtx::vertex3<T>(x, y, z));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	OpenGL ��]�s����J�����g�E�}�g���b�N�X�ɍ�������
			@param[in]	angle	0 �` 360 �x��(DEG)�p�x
			@param[in]	x	��]���S�� X �v�f
			@param[in]	y	��]���S�� Y �v�f
			@param[in]	z	��]���S�� Z �v�f
		 */
		//-----------------------------------------------------------------//
		void rotate(T angle, T x, T y, T z) {
			acc_[mode_].rotate(angle, vtx::vertex3<T>(x, y, z));
		};


		//-----------------------------------------------------------------//
		/*!
			@brief	�J�����g�E�}�g���b�N�X���Q��
			@return	OpenGL ���т́A�x�[�X�}�g���b�N�X
		 */
		//-----------------------------------------------------------------//
		mtx::matrix4<T>& at_current_matrix() { return acc_[mode_]; };


		//-----------------------------------------------------------------//
		/*!
			@brief	�J�����g�E�}�g���b�N�X�𓾂�
			@return	OpenGL ���т́A�x�[�X�}�g���b�N�X
		 */
		//-----------------------------------------------------------------//
		const mtx::matrix4<T>& get_current_matrix() const { return acc_[mode_]; };


		//-----------------------------------------------------------------//
		/*!
			@brief	�v���W�F�N�V�����E�}�g���b�N�X�𓾂�
			@return	OpenGL ���т́A�x�[�X�}�g���b�N�X
		 */
		//-----------------------------------------------------------------//
		const mtx::matrix4<T>& get_projection_matrix() const {
			return acc_[mode::projection];
		};


		//-----------------------------------------------------------------//
		/*!
			@brief	���f���E�}�g���b�N�X�𓾂�
			@return	OpenGL ���т́A�x�[�X�}�g���b�N�X
		 */
		//-----------------------------------------------------------------//
		const mtx::matrix4<T>& get_modelview_matrix() const {
			return acc_[mode::modelview];
		};


		//-----------------------------------------------------------------//
		/*!
			@brief	���[���h�E�}�g���b�N�X�i�ŏI�j���v�Z����
			@return	OpenGL ���т́A���[���h�E�}�g���b�N�X
		 */
		//-----------------------------------------------------------------//
		void world_matrix(mtx::matrix4<T>& mat) const {
			mtx::matmul4(mat.m, acc_[mode::projection].m, acc_[mode::modelview].m);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	���_����ϊ����ꂽ���W�𓾂�
			@param[in]	mat		�x�[�X�E�}�g���b�N�X
			@param[in]	inv		���_
			@param[out]	out		�ϊ����ꂽ���W
			@param[out]	scr		�X�N���[�����W
		 */
		//-----------------------------------------------------------------//
		void vertex(const mtx::matrix4<T>& mat, const vtx::vertex3<T>& inv, vtx::vertex3<T>& out, vtx::vertex3<T>& scr) const {
			vtx::vertex4<T> in = inv;
			T o[4];
			mtx::matmul1<T>(o, mat.m, in.getXYZW());
			out.set(o[0], o[1], o[2]);
			T invw = static_cast<T>(1) / o[3];
			T w = (far_ * near_) / (far_ - near_) * invw;
			scr.set(out.x * invw, out.y * invw, w);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	���_����ϊ����ꂽ���[���h���W�𓾂�
			@param[in]	mat		�x�[�X�E�}�g���b�N�X
			@param[in]	inv		���_
			@param[out]	out		���ʂ��󂯎��x�N�^�[
		 */
		//-----------------------------------------------------------------//
		static void vertex_world(const mtx::matrix4<T>& mat, const vtx::vertex3<T>& inv, vtx::vertex4<T>& out) {
			vtx::vertex4<T> in = inv;
			T o[4];
			mtx::matmul1<T>(o, mat(), in.getXYZW());
			out.set(o[0], o[1], o[2], o[3]);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	���_���琳�K�����ꂽ�X�N���[�����W�𓾂�
			@param[in]	mat		�x�[�X�E�}�g���b�N�X
			@param[in]	inv		���_
			@param[out]	outv	���ʂ��󂯎��x�N�^�[
		 */
		//-----------------------------------------------------------------//
		void vertex_screen(const mtx::matrix4<T>& mat, const vtx::vertex3<T>& inv, vtx::vertex3<T>& outv) const {
			T out[4];
			vtx::vertex4<T> in = inv;
			mtx::matmul1<T>(out, mat.m, in.getXYZW());
			T invw = 1.0f / out[3];
			T w = (far_ * near_) / (far_ - near_) * invw;
			outv.set(out[0] * invw, out[1] * invw, w);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	�}�E�X���W�𐳋K������
			@param[in]	mspos	�}�E�X�ʒu�i���オ0,0�j
			@param[in]	rpos	���K�����ꂽ�ʒu
		 */
		//-----------------------------------------------------------------//
		void regularization_mouse_position(const vtx::spos& mspos, vtx::vertex2<T>& rpos) const {
			T fw = static_cast<T>(vp_w_) / static_cast<T>(2);
			T fh = static_cast<T>(vp_h_) / static_cast<T>(2);
			rpos.set((static_cast<float>(mspos.x) - fw) / fw, (fh - static_cast<float>(mspos.y)) / fh);
		}


#if 0
		//-----------------------------------------------------------------//
		/*!
			@brief	�J�����g�E�}�g���b�N�X�̕\��
		 */
		//-----------------------------------------------------------------//
		void print_matrix() {
			for(int i = 0; i < 4; ++i) {
				std::cout << boost::format("(%d) %-1.5f, %-1.5f, %-1.5f, %-1.5f\n")
					% i
					% acc_[mode_].m[0 * 4 + i]
					% acc_[mode_].m[1 * 4 + i]
					% acc_[mode_].m[2 * 4 + i]
					% acc_[mode_].m[3 * 4 + i];
			}
		}
#endif
	};

	typedef matrix<float>	matrixf;
	typedef matrix<double>	matrixd;	
}
