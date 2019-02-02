;; Lepton EDA
;; liblepton - Lepton's library - Scheme API
;; Copyright (C) 2019 dmn <graahnul.grom@gmail.com>
;; License: GPLv2+. See the COPYING file
;;

( define-module ( lepton legacy-config )

  #:use-module  ( ice-9 format )
  #:use-module  ( geda  config )
  #:use-module  ( lepton legacy-config keylist )

  #:export      ( config-upgrade )
)



; public:
;
; config-upgrade(): upgrade legacy gEDA configuration
;
; Read legacy gEDA configuration from geda.conf (in current
; directory), geda-user.conf or geda-system.conf file,
; convert it (using new names) and produce the corresponding
; lepton*.conf configuration file.
; We get conversion information (list of keys, old and new names) from
; the list structure defined in the (lepton legacy-config keylist) module.
;
; [what]:                'local, 'user or 'system - config to convert
; [report-absent-keys]:  print messages about missing keys in old cfg file
; [overwrite]:           if #t, overwrite existing new cfg file
; {thr}:                 'ctx 'infile 'load 'outfile 'save
; {ret}:                 new cfg file full name or #f on failure
;
; This function might throw the following exceptions:
; - 'ctx:      cannot get configuration context
; - 'infile:   legacy config file does not exist
; - 'load:     cannot read legacy config file
; - 'outfile:  output config file already exists
; - 'save:     cannot write output config file
;
( define* ( config-upgrade what #:key (report-absent-keys #f) (overwrite #f) )
( let
  (
  ( cfg    #f )
  ( fname  #f )
  ( keys  '() )
  ( data  '() )
  )

  ;
  ; read legacy config file (geda*.conf):
  ;

  ( config-set-legacy-mode! #t ) ; use geda*.conf files

  ( set! cfg ( get-cfg-ctx what ) )
  ( unless cfg
    ( throw 'ctx "Cannot get config context (input)" )
  )

  ( set! fname (config-filename cfg) )
  ( unless ( and fname (access? fname F_OK) )
    ( throw 'infile "Input config file does not exist:" fname )
  )

  ( format #t "ii: INPUT: ~a~%" fname )
  ( format #t "~%" )

  ( catch #t
    ( lambda()
      ( config-load! cfg )
      ( set! keys (config-keylist) )
      ( set! data
        ( read-old cfg keys #:report-absent-keys report-absent-keys )
      )
    )
    ( lambda( ex . args )
      ( throw 'load "Cannot read config:" ex args )
    )
  )


  ;
  ; write new config file (lepton*.conf):
  ;

  ( format #t "~%" )

  ( config-set-legacy-mode! #f ) ; use lepton*.conf files

  ( set! cfg ( get-cfg-ctx what ) )
  ( unless cfg
    ( throw 'ctx "Cannot get config context (output)" )
  )

  ( set! fname (config-filename cfg) )
  ( when ( and fname (access? fname F_OK) (not overwrite) )
    ( throw 'outfile "Output config file already exists:" fname )
  )

  ( format #t "ii: OUTPUT: ~a~%" fname )

  ( catch #t
    ( lambda()
      ( write-new cfg data )
      ( config-save! cfg )
    )
    ( lambda( ex . args )
      ( throw 'save "Cannot write config:" ex args )
    )
  )

  ; return:
  fname

) ; let
) ; config-upgrade()



; get configuration context
;
; [what]: symbol: 'local, 'user or 'system
; {ret}:  requested configuration context or #f on failure
;
( define ( get-cfg-ctx what )
  ; return:
  ( cond
    (  ( eq? what 'local  )  ( path-config-context (getcwd) )  )
    (  ( eq? what 'user   )  ( user-config-context )           )
    (  ( eq? what 'system )  ( system-config-context )         )
    (  else #f  )
  )
)



; read legacy configuration
;
; [cfg]:                 configuration context to read from
; [keys]:                cfg keys list as defined in (conf keys) module
; [report-absent-keys]:  report absent keys
; {ret}:                 list of entries: ( grp-new key-new val )
;
( define* ( read-old cfg keys #:key (report-absent-keys #f) )
( let
  (
  ( grp-old  #f )
  ( key-old  #f )
  ( grp-new  #f )
  ( key-new  #f )
  ( pfn      #f )
  ( val      #f )
  ( res     '() )
  )

  ( define ( parse-entry e )
    ( set! pfn     (list-ref e 0) )
    ( set! grp-old (list-ref e 2) )
    ( set! key-old (list-ref e 3) )
    ( set! grp-new (list-ref e 4) )
    ( set! key-new (list-ref e 5) )
  )

  ( define ( add-to-res g k v ) ; group, key, value
    ( set! res ( cons (list g k v) res ) )
  )

  ( for-each
  ( lambda( entry )

    ( parse-entry entry )

    ( catch #t
      ( lambda()
        ( set! val ( pfn cfg grp-old key-old ) ) ; pfn() throws
        ( format #t "ii: read   [~a]::~a ~65,4t = [~a]~%" grp-old key-old val )
        ( add-to-res grp-new key-new val )
      )
      ( lambda( ex . args )
        ( if report-absent-keys
          ( format #t "ww: !read  [~a]::~a~%" grp-old key-old )
        )
      )
    )

  )
  keys
  )

  ; return:
  res

) ; let
) ; read-old()



; write new configuration
;
; [cfg]: configuration context to write to
; [res]: list returned by read-old()
;
( define ( write-new cfg res )
( let
  (
  ( grp-new #f )
  ( key-new #f )
  ( val     #f )
  )

  ( define ( parse-entry e )
    ( set! grp-new (list-ref e 0) )
    ( set! key-new (list-ref e 1) )
    ( set! val     (list-ref e 2) )
  )

  ( for-each
  ( lambda( entry )
    ( parse-entry entry )
    ( set-config! cfg grp-new key-new val ) ; TODO: throws?
  )
  res
  )

) ; let
) ; write-new()



; vim: ft=scheme tabstop=2 softtabstop=2 shiftwidth=2 expandtab

