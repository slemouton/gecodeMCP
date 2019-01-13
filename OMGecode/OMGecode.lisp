
;--------------------------------------------------
;Package Definition (Optional, else use package :OM) 
;--------------------------------------------------

(in-package :om)

 
;--------------------------------------------------
;Variable definiton with files to load 
;--------------------------------------------------

(defvar *omgecode-files* nil)
(setf  *omgecode-files* '(
                        "frameworkGlue"
                        "slmGecode"
                         ))

;--------------------------------------------------
;Loading files 
;--------------------------------------------------

(mapc #'(lambda (file) 
          (compile&load (make-pathname :directory (append (pathname-directory *load-pathname*) '("OMGecode-sources")) :name file))) *omgecode-files*)

;--------------------------------------------------
; OM subpackages initialization
; ("sub-pack-name" subpacke-lists class-list function-list class-alias-list)
;--------------------------------------------------



(defvar *subpackages-omgecode* nil)
(setf *subpackages-omgecode*
      '(("PROBLEMS" (("jarrell1" nil nil (jarrell1) nil)
                     ("all-interval-g" nil nil (all-interval-g) nil)
                     ("hamming-g" nil nil (hamming-g) nil)
                     ))))
  

;--------------------------------------------------
;filling packages
;--------------------------------------------------
(om::fill-library *subpackages-omgecode*)

(om::set-lib-release 6.0)

