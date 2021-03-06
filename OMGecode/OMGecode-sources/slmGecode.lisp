;;; serge lemouton - octobre 2009
;;;
;;; todo 

;sequences de contraintes (cf gelisp:jarrell-ex2)
;intervalles interdit ex : (-1(0)) -> pas de note repet�es
;interpolation d'accord par substitution de notes avec contraintes sur les intervalles autoris�s/interdits, et sur le parcours des notes extr�mes
; possible de lancer le calcul dans un trhead separ� ?



(in-package 'om)

(defmethod! vps-g ((nnotes number)(firstNote number)(lastNote number)(solutions number)
                           &KEY (density "(0 0)") (cs "(0 0)") (domain "(0)") (timelimit 0))
            :icon 669
            :doc "constraints on vertical pitch structures"
              (read-from-string(c-vpsG nnotes firstNote lastNote solutions :density density :cs cs :domain domain :timelimit timelimit)))

(defmethod! hamming-g ((nnotes number)(distance number)(nchords number)(solutions number)
                            &KEY(timelimit 0))
            :icon 669
            :doc "genere une suite d'accord qui different tous d'au moins d notes"
              (read-from-string(c-hamming nnotes distance nchords solutions :timelimit timelimit)))

(defmethod! all-interval-g ((size number)(solutions number)
                            &KEY(timelimit 0)(model 0)(symmetry 0))
            :icon 669
            :doc "all-interval-series size number_of_solutions"
              (read-from-string(c-allintervalG size solutions :timelimit timelimit :model model :symmetry symmetry)))

(defmethod! gechord-g ((size number)(cells list)(first-note number)(last-note number)(solutions number)
                            &KEY(timelimit 0) (model 0))
            :icon 669
            :doc "genere des accords avec les intervalles souhait�s ou interdits"
            (let((s1 (write-to-string cells)))
              (read-from-string(c-gechord size s1 first-note last-note solutions :timelimit timelimit :model model))))

(defmethod! chsort-g ((size number)(chords list)(solutions number)
                            &KEY(timelimit 0) (model 0))
            :icon 669
            :doc "trouve le parcours avec le plus de notes communes entre les accords"
            (let((s1 (write-to-string chords)))
              (read-from-string(c-chsort size s1 solutions :timelimit timelimit :model model))))

(defmethod! profils-g ((size number)(bpfs list)(nn number)(solutions number)
                            &KEY(timelimit 0)(min 2)(max 5))
            :icon 669
            :doc "genere des accords avec les voix extremes control�es par bpf"
            (let((s1 (write-to-string bpfs)))
              (read-from-string(c-profils size s1 nn solutions :timelimit timelimit :min min :max max))))

(defmethod! interpol-g ((start list)(end list)(domain list)(forbiddenI list)(steps number)(size number)(solutions number)
                            &KEY(timelimit 0))
            :icon 669
            :doc "interpolations melodiques avec contraintes d'intervalles"
            (let((s1 (write-to-string start))
                 (s2 (write-to-string end))
                 (s3 (write-to-string domain))
                 (s4 (write-to-string forbiddenI)))
              (read-from-string(c-interpol s1 s2 s3 s4 steps size solutions :timelimit timelimit))))

(defmethod* jarrell1 ((n number)(cells list)(chord list)(first-note number)(last-note number))
            :icon 669
            :doc "jarrell gecode solver : returns one solution"
            (let((s1 (write-to-string cells))
                 (s2 (write-to-string chord)))
              (read-from-string(c-jarrell n  s1 s2 first-note last-note))))

(defmethod* jarrell2 ((n number)(cells list)(chord list)(first-note number)(last-note number)(n_solutions number)(filename string))
            :icon 669
            :doc "jarrell gecode solver : returns one solution and write n_solutions in a file called filename"
            (let((s1 (write-to-string cells))
                 (s2 (write-to-string chord)))
              (read-from-string(c-jarrell2 n s1 s2 first-note last-note n_solutions filename))))

(defmethod! jarrell3 ((n number)(cells list)(chord list)(first-note number)(last-note number)(n_solutions number)
                      &KEY(timelimit 10000)(model 1)(searchengine 1) (filename "tmp/tmpjarrell3"))
            :icon 669  ;; icon ??
            :initvals '(15 '(1 (1 -1)(2 (2 -2))) '(60 61 62 63 64 65 66 67) 60 60 1 :timelimit 100000)
            :doc "jarrell gecode solver : returns one solution and write n_solutions in a file called filename, available options : model, timelime
model : 0 pas de contrainte sur les intervalles, 1 : tous les intervalles de la solution doivent appartenir au cellules
timelimit : en millisecondes
searchengine : 0 : toutes les solutions, 1: cherche la meilleure solution
"
            (let((s1 (write-to-string cells))
                 (s2 (write-to-string chord)))
              (read-from-string(c-jarrellG n s1 s2 first-note last-note n_solutions :timelimit timelimit :model model :searchengine searchengine :outfile filename))))

;(c-jarrell 16 "((2(-3 -8))(1(3 8))(1(6 -6)))" "(60 65 66 68 71 76 82)" 60 60)

;utilities 
(defmethod! chord_octaviation ((chord list)(low-note number)(high-note number))
            :doc "retourne toutes les notes de chord octavi�es entre low-note et high-note"
(let ((pitchset(mapcar(lambda(x)(mod x 1200)) chord)))
(loop for i from low-note to high-note
when (member (mod i 1200) pitchset)
collect i)))

(defmethod! sort-by-center_ ((chords list))
            :doc "trie les accords par centre de gravit� ascendant"
(sort chords #'(lambda (x y) (> (apply #'+ x) (apply #'+ y)))))

(defmethod! remove-list-dup ((listes list))
            :doc "retire les accords doublons"
 
    (loop for l on listes
          do (setf is-dup nil)
          do (loop for l2 in (cdr l)
                
                do (if (equal (car l) l2) (progn(print (list (car l) l2))(setf is-dup t ))))
           when (not is-dup) collect (car l)))


;(remove-list-dup '((3 4)(4 8)(1 2)(1 2)(5 6)(1 2)))
