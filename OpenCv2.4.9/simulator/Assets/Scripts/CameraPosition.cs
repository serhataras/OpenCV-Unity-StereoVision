using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraPosition : MonoBehaviour {

    public float spacing;
    public float angle;
    private Vector3 LPosition;
    private Vector3 RPosition;

    public Rigidbody LCamera;
    public Rigidbody RCamera;

    void Awake () {
        LCamera.MovePosition(LCamera.position  + new Vector3( - spacing / 2, 0, 0));
        RCamera.MovePosition(RCamera.position + new Vector3(   spacing / 2, 0, 0));
    }

    void Start()
    {
        LPosition = LCamera.position;
        RPosition = RCamera.position;
    }

    void FixedUpdate()
    {
        /*LCamera.position = LPosition;
        RCamera.position = RPosition;*/
    }

    // Update is called once per frame
    void Update () {
		
	}
}
