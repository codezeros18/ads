#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Post {
    int id;
    char content[256];
    int likes;
    struct Post* next;
} Post;

Post* head = NULL;
int post_count = 0;

void uploadPost(char* content) {
    Post* newPost = (Post*)malloc(sizeof(Post));
    newPost->id = ++post_count;
    strcpy(newPost->content, content);
    newPost->likes = 0;
    newPost->next = head;
    head = newPost;
    printf("Post #%d uploaded: %s\n", newPost->id, newPost->content);
}

void deletePost(int id) {
    Post *temp = head, *prev = NULL;
    while (temp != NULL && temp->id != id) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) {
        printf("Post not found!\n");
        return;
    }
    if (prev == NULL)
        head = temp->next;
    else
        prev->next = temp->next;
    free(temp);
    printf("Post #%d deleted.\n", id);
}


void modifyLikes(int id, int action) {
    Post* temp = head;
    while (temp != NULL) {
        if (temp->id == id) {
            if (action == 1) {  
                temp->likes++;
                printf("Post #%d liked! Total likes: %d\n", id, temp->likes);
            } else if (action == -1 && temp->likes > 0) {  
                printf("Post #%d unliked! Total likes: %d\n", id, temp->likes);
            } else {
                printf("Post #%d has no likes to remove!\n", id);
            }
            return;
        }
        temp = temp->next;
    }
    printf("Post not found!\n");
}


void displayPosts() {
    Post* temp = head;
    if (temp == NULL) {
        printf("No posts available.\n");
        return;
    }
    while (temp != NULL) {
        printf("Post #%d: %s [Likes: %d]\n", temp->id, temp->content, temp->likes);
        temp = temp->next;
    }
}

int main() {
    int choice, id, action;
    char content[256];
    while (1) {
        printf("\n1. Upload Post\n2. Delete Post\n3. Modify Likes (Like/Unlike)\n4. Display Posts\n5. Exit\nEnter choice: ");
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1:
                printf("Enter post content: ");
                fgets(content, 256, stdin);
                content[strcspn(content, "\n")] = 0;
                uploadPost(content);
                break;
            case 2:
                printf("Enter post ID to delete: ");
                scanf("%d", &id);
                deletePost(id);
                break;
            case 3:
                printf("Enter post ID to modify likes: ");
                scanf("%d", &id);
                printf("Enter 1 to Like, -1 to Unlike: ");
                scanf("%d", &action);
                modifyLikes(id, action);
                break;
            case 4:
                displayPosts();
                break;
            case 5:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}
