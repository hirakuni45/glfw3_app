;;
;;  GNU emacs (.emacs) 設定ファイル
;;
(setq inhibit-startup-message t)

;; 言語を日本語にする
(set-language-environment 'Japanese)
;; 極力UTF-8 とする
(prefer-coding-system 'utf-8)

;;; IME
(when (fboundp 'mw32-ime-initialize)
  (setq default-input-method "MW32-IME")
  (setq-default mw32-ime-mode-line-state-indicator "[--]")
  (setq mw32-ime-mode-line-state-indicator-list '("[--]" "[あ]" "[--]"))
  (mw32-ime-initialize)

  (wrap-function-to-control-ime 'y-or-n-p nil nil)
  (wrap-function-to-control-ime 'yes-or-no-p nil nil)
  (wrap-function-to-control-ime 'universal-argument t nil)
  (wrap-function-to-control-ime 'read-string nil nil)
  (wrap-function-to-control-ime 'read-from-minibuffer nil nil)
  (wrap-function-to-control-ime 'read-key-sequence nil nil)
  (wrap-function-to-control-ime 'map-y-or-n-p nil nil)
  (wrap-function-to-control-ime 'read-passwd t t) ; don't work as we expect.

  (add-hook 'mw32-ime-on-hook
	    (function (lambda () (set-cursor-color "blue"))))
  (add-hook 'mw32-ime-off-hook
	    (function (lambda () (set-cursor-color "black"))))
  (add-hook 'minibuffer-setup-hook
  	    (function (lambda ()
  			(if (fep-get-mode)
			    (set-cursor-color "blue")
			  (set-cursor-color "black"))))))

 ;;; 標準フォント
;;(set-default-font "Consolas 11")

(set-fontset-font (frame-parameter nil 'font)
                  'japanese-jisx0208
                  '("ＭＳ ゴシック" . "unicode-bmp")
                  )
(set-fontset-font (frame-parameter nil 'font)
                  'katakana-jisx0201
                  '("ＭＳ ゴシック" . "unicode-bmp")
                  )

(setq next-line-add-newlines nil)
(line-number-mode t)

;; 検索の場合に、大文字／小文字を厳密に比べる。
(setq-default case-fold-search nil)
(setq-default case-replace nil)

;; 個人的なキーバインドの設定（平松用）
(global-set-key "\C-z" 'scroll-down)
(global-set-key "\C-xx" 'switch-to-buffer)
(global-set-key "\M-g" 'goto-line)
(global-set-key "\M-r" 'replace-string)
(global-set-key "\M-:" 'replace-regxp)
(global-set-key "\M-;" 'set-file-coding-system)
(define-key function-key-map [delete] [4])
;; (put 'delete 'ascii-character 4)

(setq default-tab-width 4)

;; モードライン／フレームのディフォルト設定。
(setq default-frame-alist
      (append (list '(foreground-color . "black") ; black
                    '(background-color . "antique white") ; LemonChiffon
;;                    '(background-color . "gray") ; gray
                    '(border-color . "white") ; black
                    '(mouse-color . "cyan") ; white
                    '(cursor-color . "dark gray") ; black
;;                    '(ime-font . "Nihongo-12") ; TrueType のみ
;;                    '(font . "bdf-fontset")    ; BDF
;;                    '(font . "private-fontset"); TrueType
                    '(width . 112)
                    '(height . 38)
                    '(top . 8)
                    '(left . 8))
              default-frame-alist))

;; メニュー・バーを使わない。
(menu-bar-mode 0)
;; ツールバーを消す
;; (tool-bar-mode -1)

;; 音を消す
;;(set-message-beep 'silent)

;; TAB 文字とかの代替表示
;(defface my-face-b-1 '((t (:background "medium aquamarine"))) nil)
;(defface my-face-b-2 '((t (:background "#d2d3bb"))) nil)
;(defface my-face-u-1 '((t (:foreground "SteelBlue" :underline t))) nil)
;(defvar my-face-b-1 'my-face-b-1)
;(defvar my-face-b-2 'my-face-b-2)
;(defvar my-face-u-1 'my-face-u-1)
;(defadvice font-lock-mode (before my-font-lock-mode ())
;  (font-lock-add-keywords
;   major-mode
;   '(
;     ("　" 0 my-face-b-1 append)
;     ("\t" 0 my-face-b-2 append)
;     ("[ ]+$" 0 my-face-u-1 append)
;     )))
;(ad-enable-advice 'font-lock-mode 'before 'my-font-lock-mode)
;(ad-activate 'font-lock-mode)
;(add-hook 'find-file-hooks '(lambda ()
;(if font-lock-mode
;nil
;(font-lock-mode t))) t)

;; 前回編集時のカーソル位置を復帰
(load "saveplace")
(setq-default save-place t)

(setq c-default-style
       '((java-mode . "cc-mode")
         (other . "c-mode")))

(defun c-mode-common-hooks()
       (local-set-key "\C-m" 'newline)
       (local-set-key "\t" 'self-insert-command)
       (c-toggle-electric-state -1)
       (setq c-electric-pound-behavior nil))
(add-hook 'c-mode-common-hook 'c-mode-common-hooks)

;; いつでも TAB でインデント
(setq c-tab-always-indent t)
(setq c-tab-always-indent nil)

;; 終了
