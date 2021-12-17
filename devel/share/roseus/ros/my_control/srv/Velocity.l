;; Auto-generated. Do not edit!


(when (boundp 'my_control::Velocity)
  (if (not (find-package "MY_CONTROL"))
    (make-package "MY_CONTROL"))
  (shadow 'Velocity (find-package "MY_CONTROL")))
(unless (find-package "MY_CONTROL::VELOCITY")
  (make-package "MY_CONTROL::VELOCITY"))
(unless (find-package "MY_CONTROL::VELOCITYREQUEST")
  (make-package "MY_CONTROL::VELOCITYREQUEST"))
(unless (find-package "MY_CONTROL::VELOCITYRESPONSE")
  (make-package "MY_CONTROL::VELOCITYRESPONSE"))

(in-package "ROS")





(defclass my_control::VelocityRequest
  :super ros::object
  :slots (_x _z ))

(defmethod my_control::VelocityRequest
  (:init
   (&key
    ((:x __x) 0.0)
    ((:z __z) 0.0)
    )
   (send-super :init)
   (setq _x (float __x))
   (setq _z (float __z))
   self)
  (:x
   (&optional __x)
   (if __x (setq _x __x)) _x)
  (:z
   (&optional __z)
   (if __z (setq _z __z)) _z)
  (:serialization-length
   ()
   (+
    ;; float32 _x
    4
    ;; float32 _z
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _x
       (sys::poke _x (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _z
       (sys::poke _z (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _x
     (setq _x (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _z
     (setq _z (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(defclass my_control::VelocityResponse
  :super ros::object
  :slots (_x _z ))

(defmethod my_control::VelocityResponse
  (:init
   (&key
    ((:x __x) 0.0)
    ((:z __z) 0.0)
    )
   (send-super :init)
   (setq _x (float __x))
   (setq _z (float __z))
   self)
  (:x
   (&optional __x)
   (if __x (setq _x __x)) _x)
  (:z
   (&optional __z)
   (if __z (setq _z __z)) _z)
  (:serialization-length
   ()
   (+
    ;; float32 _x
    4
    ;; float32 _z
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _x
       (sys::poke _x (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _z
       (sys::poke _z (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _x
     (setq _x (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _z
     (setq _z (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(defclass my_control::Velocity
  :super ros::object
  :slots ())

(setf (get my_control::Velocity :md5sum-) "57be304b041c9bb67d96dd30f6852c26")
(setf (get my_control::Velocity :datatype-) "my_control/Velocity")
(setf (get my_control::Velocity :request) my_control::VelocityRequest)
(setf (get my_control::Velocity :response) my_control::VelocityResponse)

(defmethod my_control::VelocityRequest
  (:response () (instance my_control::VelocityResponse :init)))

(setf (get my_control::VelocityRequest :md5sum-) "57be304b041c9bb67d96dd30f6852c26")
(setf (get my_control::VelocityRequest :datatype-) "my_control/VelocityRequest")
(setf (get my_control::VelocityRequest :definition-)
      "float32 x
float32 z
---
float32 x
float32 z

")

(setf (get my_control::VelocityResponse :md5sum-) "57be304b041c9bb67d96dd30f6852c26")
(setf (get my_control::VelocityResponse :datatype-) "my_control/VelocityResponse")
(setf (get my_control::VelocityResponse :definition-)
      "float32 x
float32 z
---
float32 x
float32 z

")



(provide :my_control/Velocity "57be304b041c9bb67d96dd30f6852c26")


